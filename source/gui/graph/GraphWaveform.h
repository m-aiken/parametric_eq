#pragma once

#include <JuceHeader.h>

#include "GraphPathProducer.h"
#include "MonoBufferFifo.h"

class GraphWaveform final
    : public juce::Component
    , public juce::Timer
{
public:
    GraphWaveform(const double        sample_rate,
                  MonoBufferFifo&     left_scsf,
                  MonoBufferFifo&     right_scsf,
                  const juce::Colour& colour);
    ~GraphWaveform() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    juce::Colour      colour_;
    juce::Path        left_analyser_path_;
    juce::Path        right_analyser_path_;
    GraphPathProducer left_path_producer_;
    GraphPathProducer right_path_producer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphWaveform)
};
