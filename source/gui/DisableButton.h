#pragma once

#include <JuceHeader.h>

#include "ParametricEqProcessor.h"

class DisableButton final : public juce::ToggleButton
{
public:
    explicit DisableButton(ParametricEqProcessor& processor_ref);

    void paintButton(juce::Graphics& g, bool should_draw_button_as_highlighted, bool should_draw_button_as_down) override;

private:
    ParametricEqProcessor&                               processor_ref_;
    juce::AudioProcessorValueTreeState::ButtonAttachment button_attachment_;

    static constexpr float ICON_LINE_THICKNESS = 1.f;
    static constexpr float ORB_DIAMETER        = 6.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DisableButton)
};
