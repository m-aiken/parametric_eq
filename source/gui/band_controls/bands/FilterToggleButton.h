#pragma once

#include <JuceHeader.h>

#include "ParametricEqGlobals.h"
#include "ParametricEqProcessor.h"

class FilterToggleButton final
    : public juce::ToggleButton
    , public juce::AudioProcessorValueTreeState::Listener
{
public:
    FilterToggleButton(ParametricEqProcessor& processor_ref, const ParametricEq::FILTER_ID& filter_id);
    ~FilterToggleButton() override;

    void paintButton(juce::Graphics& g, bool should_draw_button_as_highlighted, bool should_draw_button_as_down) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;
    void parameterChanged(const juce::String& parameter_id, float new_value) override;

private:
    void setEnablement();

    ParametricEqProcessor&                               processor_ref_;
    const ParametricEq::FILTER_ID                        filter_id_;
    juce::AudioProcessorValueTreeState::ButtonAttachment button_attachment_;

    juce::Colour background_colour_;

    static constexpr float LINE_THICKNESS      = 1.f;
    static constexpr float HALF_LINE_THICKNESS = LINE_THICKNESS / 2;
    static constexpr float CORNER_RADIUS       = 2.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterToggleButton)
};
