#pragma once

#include <JuceHeader.h>

class GraphNode final : public juce::ToggleButton
{
public:
    GraphNode(juce::AudioProcessorValueTreeState& apvts, const juce::String& band_enablement_parameter);

    void paint(juce::Graphics& g) override;

    juce::Point< int >& coordinates();

    void setIsActiveBand(const bool is_active);

    enum {
        DIAMETER             = 12,
        RADIUS               = DIAMETER / 2,
        CENTRE_DIAMETER      = 6,
        OUTER_RING_THICKNESS = 1,
    };

private:
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment_;

    juce::Point< int > coordinates_;
    bool               is_active_band_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
