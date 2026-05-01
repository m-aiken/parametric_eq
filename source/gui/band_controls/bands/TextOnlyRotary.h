#pragma once

#include <JuceHeader.h>

#include "TextOnlyRotaryLnf.h"

class TextOnlyRotary final : public juce::Slider
{
public:
    TextOnlyRotary(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameter_id);
    ~TextOnlyRotary() override;

    void mouseDoubleClick(const juce::MouseEvent& event) override;

    void setEnablement(const bool enable);

private:
    TextOnlyRotaryLnf lnf_;

    juce::AudioProcessorValueTreeState::SliderAttachment slider_attachment_;
    juce::RangedAudioParameter*                          param_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextOnlyRotary)
};
