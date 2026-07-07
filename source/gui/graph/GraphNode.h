#pragma once

#include <JuceHeader.h>

class GraphNode final : public juce::ToggleButton
{
public:
    GraphNode(juce::AudioProcessorValueTreeState& apvts, const juce::String& band_enablement_parameter);

    void paint(juce::Graphics& g) override;

    juce::Point< float >& coordinates();

    void setIsActiveBand(const bool is_active);

    static constexpr float DIAMETER             = 16.f;
    static constexpr float RADIUS               = DIAMETER / 2.f;
    static constexpr float CENTRE_DIAMETER      = 8.f;
    static constexpr float OUTER_RING_THICKNESS = 2.f;

private:
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment_;
    juce::Point< float >                                 coordinates_;
    bool                                                 is_active_band_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
