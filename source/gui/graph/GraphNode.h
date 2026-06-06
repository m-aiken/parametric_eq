#pragma once

#include <JuceHeader.h>

#include "ParametricEqProcessor.h"

class GraphNode final
    : public juce::ToggleButton
    , public juce::AudioProcessorValueTreeState::Listener
{
public:
    GraphNode(ParametricEqProcessor& processor_ref, const juce::String& band_enablement_parameter);
    ~GraphNode() override;

    void paint(juce::Graphics& g) override;
    void parameterChanged(const juce::String& parameter_id, float new_value) override;

    juce::Point< int >& coordinates();

    void setIsActiveBand(const bool is_active);

    enum {
        DIAMETER             = 12,
        RADIUS               = DIAMETER / 2,
        CENTRE_DIAMETER      = 6,
        OUTER_RING_THICKNESS = 1,
    };

private:
    void setEnablement();

    ParametricEqProcessor&                               processor_ref_;
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment_;
    juce::Point< int >                                   coordinates_;
    bool                                                 is_active_band_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphNode)
};
