#pragma once

#include <JuceHeader.h>

#include "MonoBufferFifo.h"
#include "PathProducer.h"

class SpectrumAnalyser final
    : public juce::Component
    , public juce::Timer
{
public:
    SpectrumAnalyser(const double        sample_rate,
                     MonoBufferFifo&     left_scsf,
                     MonoBufferFifo&     right_scsf,
                     const juce::Colour& colour);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    void animate();

    juce::Colour colour_;
    juce::Path   left_analyser_path_;
    juce::Path   right_analyser_path_;
    PathProducer left_path_producer_;
    PathProducer right_path_producer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyser)
};
