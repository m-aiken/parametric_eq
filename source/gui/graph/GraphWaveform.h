#pragma once

#include <JuceHeader.h>

#include "FftInputDataFifo.h"
#include "ParametricEqFft.h"
#include "ParametricEqProcessor.h"

class GraphWaveform final
    : public juce::Component
    , public juce::Timer
{
public:
    GraphWaveform(ParametricEqProcessor& processor_ref, const juce::Colour& colour);
    ~GraphWaveform() override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

private:
    void performFft();
    void smoothFftOutput();
    void generatePath();
    void shutdown();

    ParametricEqProcessor& processor_ref_;

    juce::Colour colour_;
    juce::Path   path_;

    // FFT.
    FftInputDataFifo&                     fft_input_data_fifo_;
    juce::dsp::FFT                        fft_;
    juce::dsp::WindowingFunction< float > windowing_func_;
    juce::AudioBuffer< float >            fft_input_buffer_;
    parametric_eq::fft::OutputBuffer      fft_output_buffer_;

    // Smoothed FFT output.
    parametric_eq::fft::OutputBuffer smoothed_fft_output_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphWaveform)
};
