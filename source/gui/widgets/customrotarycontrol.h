#pragma once

#include <JuceHeader.h>

class CustomRotaryControl final : public juce::Slider
{
public:
    CustomRotaryControl(juce::RangedAudioParameter* rap, const juce::String& title);

    void                                 paint(juce::Graphics& g) override;
    [[nodiscard]] juce::Rectangle< int > getRotaryBounds() const;

private:
    juce::RangedAudioParameter* param;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomRotaryControl)
};
