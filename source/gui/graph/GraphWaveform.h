#pragma once

#include <JuceHeader.h>

#include "FftInputDataFifo.h"
#include "GraphicEqGlobals.h"

class GraphWaveform final
    : public juce::Component
    , public juce::Timer
{
public:
    GraphWaveform(const double sample_rate, FftInputDataFifo& fft_input_data_fifo, const juce::Colour& colour);
    ~GraphWaveform() override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

private:
    void performFft();
    void smoothFftOutput();
    void generatePath();

    juce::Colour colour_;
    juce::Path   path_;
    double       sample_rate_;

    // FFT.
    FftInputDataFifo&                     fft_input_data_fifo_;
    juce::dsp::FFT                        fft_;
    juce::dsp::WindowingFunction< float > windowing_func_;
    juce::AudioBuffer< float >            fft_input_buffer_;
    GraphicEq::FFT::OutputBuffer          fft_output_buffer_;

    // Smoothed FFT output.
    GraphicEq::FFT::OutputBuffer smoothed_fft_output_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphWaveform)
};
