#pragma once

#include <JuceHeader.h>

#include "ParametricEqProcessor.h"

class WaveformButton final
    : public juce::ToggleButton
    , public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit WaveformButton(ParametricEqProcessor& processor_ref);
    ~WaveformButton() override;

    void paintButton(juce::Graphics& g, bool should_draw_button_as_highlighted, bool should_draw_button_as_down) override;
    void parameterChanged(const juce::String& parameter_id, float new_value) override;

private:
    void setEnablement();

    ParametricEqProcessor&                               processor_ref_;
    juce::AudioProcessorValueTreeState::ButtonAttachment button_attachment_;

    static constexpr float ICON_LINE_THICKNESS = 1.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformButton)
};
