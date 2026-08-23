#include "GraphWaveform.h"

/*---------------------------------------------------------------------------
**
*/
GraphWaveform::GraphWaveform(ParametricEqProcessor& processor_ref, const juce::Colour& colour)
    : processor_ref_(processor_ref)
    , colour_(colour)
    , fft_input_data_fifo_(processor_ref.getFftFifo())
    , fft_(parametric_eq::fft::ORDER)
    , windowing_func_(parametric_eq::fft::SIZE, juce::dsp::WindowingFunction< float >::blackmanHarris)
    , fft_output_buffer_()
    , smoothed_fft_output_()
{
    fft_input_buffer_.setSize(1, parametric_eq::fft::INPUT_BUFFER_SIZE);
    std::fill(fft_output_buffer_.begin(), fft_output_buffer_.end(), parametric_eq::fft::MIN_DB);
    std::fill(smoothed_fft_output_.begin(), smoothed_fft_output_.end(), parametric_eq::fft::MIN_DB);

    startTimerHz(parametric_eq::fft::WAVEFORM_REFRESH_RATE_HZ);
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
    const auto  gradient =
        juce::ColourGradient::vertical(colour_.withAlpha(0.f), bounds.getBottom(), colour_, bounds.getCentreY());

    g.setGradientFill(gradient);

    path_.applyTransform(juce::AffineTransform::translation(bounds.getX(), bounds.getY()));
    g.fillPath(path_);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::timerCallback()
{
    if (processor_ref_.getAudioCallbackListener().callbackIsActive()) {
        fft_input_data_fifo_.pullData(fft_input_buffer_);
    }
    else {
        fft_input_buffer_.clear();
    }

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

    if (!fft_input_buffer_.hasBeenCleared()) {
        std::copy_n(fft_input_buffer_.getReadPointer(0), parametric_eq::fft::SIZE, fft_output_buffer_.begin());
    }

    windowing_func_.multiplyWithWindowingTable(fft_output_buffer_.data(), parametric_eq::fft::SIZE);
    fft_.performFrequencyOnlyForwardTransform(fft_output_buffer_.data(), true);

    juce::FloatVectorOperations::multiply(fft_output_buffer_.data(),
                                          1.f / static_cast< float >(parametric_eq::fft::NUM_BINS),
                                          parametric_eq::fft::NUM_BINS + 1);

    for (size_t i = 0; i < static_cast< size_t >(parametric_eq::fft::NUM_BINS + 1); ++i) {
        fft_output_buffer_[i] = juce::Decibels::gainToDecibels(fft_output_buffer_[i], parametric_eq::fft::MIN_DB);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::smoothFftOutput()
{
    for (size_t i = 0; i < static_cast< size_t >(parametric_eq::fft::NUM_BINS); ++i) {
        const auto previous_value = smoothed_fft_output_[i];
        const auto candidate      = fft_output_buffer_[i];
        const auto final_value    = juce::jmax(candidate, previous_value - parametric_eq::fft::WAVEFORM_DECAY_RATE);
        smoothed_fft_output_[i]   = juce::jlimit(parametric_eq::fft::MIN_DB, parametric_eq::fft::MAX_DB, final_value);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::generatePath()
{
    const auto& path_bounds = getLocalBounds().toFloat();
    const auto  bin_width   = static_cast< float >(parametric_eq::fft::getBinWidth(processor_ref_.getSampleRate()));

    const auto bounds_bottom = path_bounds.getBottom();
    const auto bounds_y      = path_bounds.getY();
    const auto bounds_width  = path_bounds.getWidth();

    juce::Path path;
    path.preallocateSpace(3 * static_cast< int >(bounds_width));

    auto last_x_coordinate = path_bounds.getX();
    auto y_coordinate      = bounds_bottom;

    path.startNewSubPath(last_x_coordinate, y_coordinate);

    for (size_t i = 1; i < static_cast< size_t >(parametric_eq::fft::NUM_BINS + 1);
         i += parametric_eq::fft::WAVEFORM_PATH_RESOLUTION) {
        y_coordinate = juce::jmap< float >(smoothed_fft_output_.at(i),
                                           parametric_eq::fft::MIN_DB,
                                           parametric_eq::fft::MAX_DB,
                                           bounds_bottom,
                                           bounds_y);

        if (!std::isnan(y_coordinate) && !std::isinf(y_coordinate)) {
            const auto bin_freq     = static_cast< float >(i) * bin_width;
            const auto normalized_x = juce::mapFromLog10(bin_freq, parametric_eq::MIN_HZ, parametric_eq::MAX_HZ);
            const auto x_coord      = std::floor(normalized_x * bounds_width);

            if (x_coord - last_x_coordinate >= parametric_eq::fft::WAVEFORM_PATH_RESOLUTION && x_coord <= bounds_width) {
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
**
*/
void
GraphWaveform::shutdown()
{
    stopTimer();
    path_.clear();
    repaint();
}

/*---------------------------------------------------------------------------
** End of File
*/
