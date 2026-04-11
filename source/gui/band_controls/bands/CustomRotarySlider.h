#pragma once

#include <JuceHeader.h>

class CustomRotarySlider final : public juce::Slider
{
public:
    CustomRotarySlider(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameter_id);

    void                                 paint(juce::Graphics& g) override;
    [[nodiscard]] juce::Rectangle< int > getRotaryBounds() const;
    juce::RangedAudioParameter*          getParameter() const;

    void setEnablement(const bool enable);

private:
    static constexpr float START_ANGLE = juce::degreesToRadians(180.f + 45.f);
    static constexpr float END_ANGLE   = juce::degreesToRadians(180.f - 45.f) + juce::MathConstants< float >::twoPi;

    juce::AudioProcessorValueTreeState::SliderAttachment slider_attachment_;
    juce::RangedAudioParameter*                          param;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomRotarySlider)
};
