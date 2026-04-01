#pragma once

#include <JuceHeader.h>

#include "FftInputDataFifo.h"
#include "GraphPathProducer.h"

class GraphWaveform final
    : public juce::Component
    , public juce::Timer
{
public:
    GraphWaveform(const double sample_rate, FftInputDataFifo& fft_input_data_fifo, const juce::Colour& colour);
    ~GraphWaveform() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    juce::Colour      colour_;
    juce::Path        path_;
    GraphPathProducer path_producer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphWaveform)
};
