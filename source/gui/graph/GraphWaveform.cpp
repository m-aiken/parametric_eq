#include "GraphWaveform.h"

/*---------------------------------------------------------------------------
**
*/
GraphWaveform::GraphWaveform(const double sample_rate, FftInputDataFifo& fft_input_data_fifo, const juce::Colour& colour)
    : colour_(colour)
    , sample_rate_(sample_rate)
    , fft_input_data_fifo_(fft_input_data_fifo)
    , fft_(ParametricEq::FFT::ORDER)
    , windowing_func_(ParametricEq::FFT::SIZE, juce::dsp::WindowingFunction< float >::blackmanHarris)
    , fft_output_buffer_()
    , smoothed_fft_output_()
{
    fft_input_buffer_.setSize(1, ParametricEq::FFT::INPUT_BUFFER_SIZE);
    std::fill(fft_output_buffer_.begin(), fft_output_buffer_.end(), ParametricEq::FFT::MIN_DB);
    std::fill(smoothed_fft_output_.begin(), smoothed_fft_output_.end(), ParametricEq::FFT::MIN_DB);

    startTimerHz(ParametricEq::WAVEFORM_REFRESH_RATE_HZ);
}

/*---------------------------------------------------------------------------
**
*/
GraphWaveform::~GraphWaveform()
{
    stopTimer();
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::paint(juce::Graphics& g)
{
    const auto& bounds = getLocalBounds().toFloat();

    g.setColour(colour_);

    path_.applyTransform(juce::AffineTransform::translation(bounds.getX(), bounds.getY()));
    g.fillPath(path_);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::timerCallback()
{
    fft_input_data_fifo_.pullData(fft_input_buffer_);

    performFft();
    smoothFftOutput();
    generatePath();
    repaint();
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::performFft()
{
    std::fill(fft_output_buffer_.begin(), fft_output_buffer_.end(), 0.f);
    std::copy_n(fft_input_buffer_.getReadPointer(0), ParametricEq::FFT::SIZE, fft_output_buffer_.begin());

    windowing_func_.multiplyWithWindowingTable(fft_output_buffer_.data(), ParametricEq::FFT::SIZE);
    fft_.performFrequencyOnlyForwardTransform(fft_output_buffer_.data(), true);

    juce::FloatVectorOperations::multiply(fft_output_buffer_.data(),
                                          1.f / static_cast< float >(ParametricEq::FFT::NUM_BINS),
                                          ParametricEq::FFT::NUM_BINS + 1);

    for (size_t i = 0; i < static_cast< size_t >(ParametricEq::FFT::NUM_BINS + 1); ++i) {
        fft_output_buffer_[i] = juce::Decibels::gainToDecibels(fft_output_buffer_[i], ParametricEq::FFT::MIN_DB);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::smoothFftOutput()
{
    for (size_t i = 0; i < static_cast< size_t >(ParametricEq::FFT::NUM_BINS); ++i) {
        const auto previous_value = smoothed_fft_output_[i];
        const auto candidate      = fft_output_buffer_[i];
        const auto final_value    = juce::jmax(candidate, previous_value - ParametricEq::WAVEFORM_DECAY_RATE);
        smoothed_fft_output_[i]   = juce::jlimit(ParametricEq::FFT::MIN_DB, ParametricEq::FFT::MAX_DB, final_value);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::generatePath()
{
    const auto& path_bounds = getLocalBounds().toFloat();
    const auto  bin_width   = static_cast< float >(ParametricEq::FFT::getBinWidth(sample_rate_));

    const auto bounds_bottom = path_bounds.getBottom();
    const auto bounds_y      = path_bounds.getY();
    const auto bounds_width  = path_bounds.getWidth();

    juce::Path path;
    path.preallocateSpace(3 * static_cast< int >(bounds_width));

    auto last_x_coordinate = path_bounds.getX();
    auto y_coordinate      = bounds_bottom;

    path.startNewSubPath(last_x_coordinate, y_coordinate);

    for (size_t i = 1; i < static_cast< size_t >(ParametricEq::FFT::NUM_BINS + 1);
         i += ParametricEq::WAVEFORM_PATH_RESOLUTION) {
        y_coordinate = juce::jmap< float >(smoothed_fft_output_.at(i),
                                           ParametricEq::FFT::MIN_DB,
                                           ParametricEq::FFT::MAX_DB,
                                           bounds_bottom,
                                           bounds_y);

        if (!std::isnan(y_coordinate) && !std::isinf(y_coordinate)) {
            const auto bin_freq     = static_cast< float >(i) * bin_width;
            const auto normalized_x = juce::mapFromLog10(bin_freq, ParametricEq::MIN_HZ, ParametricEq::MAX_HZ);
            const auto x_coord      = std::floor(normalized_x * bounds_width);

            if (x_coord - last_x_coordinate >= ParametricEq::WAVEFORM_PATH_RESOLUTION && x_coord <= bounds_width) {
                path.lineTo(x_coord, y_coordinate);
                last_x_coordinate = x_coord;
            }
        }
    }

    path.lineTo(bounds_width, bounds_bottom);
    path.closeSubPath();

    path_ = path;
}

/*---------------------------------------------------------------------------
** End of File
*/
