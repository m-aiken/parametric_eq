#pragma once

#include <JuceHeader.h>

class TextOnlyRotary final : public juce::Slider
{
public:
    TextOnlyRotary(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameter_id);

    void paint(juce::Graphics& g) override;

    void setEnablement(const bool enable);

private:
    juce::AudioProcessorValueTreeState::SliderAttachment slider_attachment_;
    juce::RangedAudioParameter*                          param_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextOnlyRotary)
};
