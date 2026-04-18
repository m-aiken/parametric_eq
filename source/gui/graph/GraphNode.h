#pragma once

#include <JuceHeader.h>

class GraphNode final : public juce::ToggleButton
{
public:
    GraphNode(juce::AudioProcessorValueTreeState& apvts, const juce::String& band_enablement_parameter);

    void paint(juce::Graphics& g) override;

    juce::Point< int >& coordinates();

    enum {
        DIAMETER = 6,
        RADIUS   = 3,
    };

private:
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment_;

    juce::Point< int > coordinates_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
