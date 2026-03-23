#pragma once

#include <JuceHeader.h>

class TextToggleButton final : public juce::ToggleButton
{
public:
    TextToggleButton(juce::AudioProcessorValueTreeState& apvts,
                     const juce::String&                 parameter_id,
                     const juce::String&                 label_text);

    void paintButton(juce::Graphics& g, bool should_draw_button_as_highlighted, bool should_draw_button_as_down) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;

private:
    juce::AudioProcessorValueTreeState::ButtonAttachment button_attachment_;

    juce::Colour background_colour_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextToggleButton)
};
