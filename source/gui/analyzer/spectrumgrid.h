#pragma once

#include <JuceHeader.h>

class SpectrumGrid final : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumGrid)
};
