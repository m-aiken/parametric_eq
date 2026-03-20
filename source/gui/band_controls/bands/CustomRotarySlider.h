#pragma once

#include <JuceHeader.h>

class CustomRotarySlider final : public juce::Slider
{
public:
    CustomRotarySlider(juce::AudioProcessorValueTreeState& apvts,
                       const juce::String&                 parameter_id,
                       const juce::String&                 title);

    void                                 paint(juce::Graphics& g) override;
    [[nodiscard]] juce::Rectangle< int > getRotaryBounds() const;

    void setEnablement(const bool enable);

private:
    juce::AudioProcessorValueTreeState::SliderAttachment slider_attachment_;
    juce::RangedAudioParameter*                          param;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomRotarySlider)
};
