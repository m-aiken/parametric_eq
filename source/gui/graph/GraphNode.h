#pragma once

#include <JuceHeader.h>

class GraphNode final : public juce::ToggleButton
{
public:
    GraphNode(juce::AudioProcessorValueTreeState& apvts, const juce::String& band_enablement_parameter);

    void paint(juce::Graphics& g) override;

private:
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
