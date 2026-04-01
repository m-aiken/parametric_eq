#include "GraphicEqGlobals.h"
#include "GraphPathProducer.h"

/*---------------------------------------------------------------------------
**
*/
GraphPathProducer::GraphPathProducer(const double sample_rate, FftInputDataFifo& fft_input_data_fifo)
    : juce::Thread("PathProducerThread")
    , processing_enabled_(false)
    , fft_input_data_fifo_(fft_input_data_fifo)
    , fft_(GraphicEq::FFT::ORDER)
    , windowing_func_(GraphicEq::FFT::SIZE, juce::dsp::WindowingFunction< float >::blackmanHarris)
    , fft_output_buffer_()
    , smoothed_fft_output_()
    , sample_rate_(sample_rate)
{
    fft_input_buffer_.setSize(1, GraphicEq::FFT::INPUT_BUFFER_SIZE);
    std::fill(fft_output_buffer_.begin(), fft_output_buffer_.end(), 0.f);
    std::fill(smoothed_fft_output_.begin(), smoothed_fft_output_.end(), GraphicEq::MIN_DB);

    startThread();
}

/*---------------------------------------------------------------------------
**
*/
GraphPathProducer::~GraphPathProducer()
{
    notify();
    stopThread(100);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::run()
{
    while (!threadShouldExit()) {
        if (!processing_enabled_.load()) {
            (void) wait(10);
            continue;
        }

        if (threadShouldExit()) {
            break;
        }

        if (fft_input_data_fifo_.pullData(fft_input_buffer_)) {
            performFft();
            smoothFftOutput();
            generatePath();
        }

        (void) wait(10);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::pauseThread()
{
    stopThread(100);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::setPathBounds(const juce::Rectangle< float >& bounds)
{
    pauseThread();

    if (!bounds.isEmpty()) {
        path_bounds_ = bounds;
    }

    startThread();
}

/*---------------------------------------------------------------------------
**
*/
int
GraphPathProducer::getNumAvailableForReading() const
{
    return path_fifo_.getNumAvailableForReading();
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphPathProducer::pull(juce::Path& path)
{
    return path_fifo_.pull(path);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::setProcessingEnabled(const bool enable)
{
    processing_enabled_.store(enable);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::performFft()
{
    std::fill(fft_output_buffer_.begin(), fft_output_buffer_.end(), 0.f);
    std::copy_n(fft_input_buffer_.getReadPointer(0), GraphicEq::FFT::SIZE, fft_output_buffer_.begin());

    windowing_func_.multiplyWithWindowingTable(fft_output_buffer_.data(), GraphicEq::FFT::SIZE);
    fft_.performFrequencyOnlyForwardTransform(fft_output_buffer_.data(), true);

    juce::FloatVectorOperations::multiply(fft_output_buffer_.data(),
                                          1.f / static_cast< float >(GraphicEq::FFT::NUM_BINS),
                                          GraphicEq::FFT::NUM_BINS + 1);

    for (auto i = 0; i < GraphicEq::FFT::NUM_BINS + 1; ++i) {
        fft_output_buffer_[i] = juce::Decibels::gainToDecibels(fft_output_buffer_[i], GraphicEq::MIN_DB);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::smoothFftOutput()
{
    for (auto i = 0; i < GraphicEq::FFT::NUM_BINS; ++i) {
        const auto previous_value = smoothed_fft_output_[i];
        const auto candidate      = fft_output_buffer_[i];
        const auto final_value    = juce::jmax(candidate, previous_value - DECAY_RATE);
        smoothed_fft_output_[i]   = juce::jlimit(GraphicEq::MIN_DB, GraphicEq::MAX_DB, final_value);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::generatePath()
{
    const auto bin_width = static_cast< float >(GraphicEq::FFT::getBinWidth(sample_rate_));

    const auto bounds_bottom = path_bounds_.getBottom();
    const auto bounds_y      = path_bounds_.getY();
    const auto bounds_width  = path_bounds_.getWidth();

    juce::Path path;
    path.preallocateSpace(3 * static_cast< int >(bounds_width));

    auto last_x_coordinate = path_bounds_.getX();
    auto y_coordinate      = bounds_bottom;

    path.startNewSubPath(last_x_coordinate, y_coordinate);

    for (auto i = 1; i < GraphicEq::FFT::NUM_BINS + 1; i += PATH_RESOLUTION) {
        y_coordinate = juce::jmap< float >(smoothed_fft_output_.at(i),
                                           GraphicEq::MIN_DB,
                                           GraphicEq::MAX_DB,
                                           bounds_bottom,
                                           bounds_y);

        if (!std::isnan(y_coordinate) && !std::isinf(y_coordinate)) {
            const auto bin_freq     = static_cast< float >(i) * bin_width;
            const auto normalized_x = juce::mapFromLog10(bin_freq, GraphicEq::MIN_HZ, GraphicEq::MAX_HZ);
            const auto x_coord      = std::floor(normalized_x * bounds_width);

            if (x_coord - last_x_coordinate >= PATH_RESOLUTION && x_coord <= bounds_width) {
                path.lineTo(x_coord, y_coordinate);
                last_x_coordinate = x_coord;
            }
        }
    }

    path.lineTo(bounds_width, bounds_bottom);
    path.closeSubPath();

    path_fifo_.push(path);
}

/*---------------------------------------------------------------------------
** End of File
*/
