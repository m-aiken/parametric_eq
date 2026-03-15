#pragma once

#include <JuceHeader.h>

//==============================================================================
struct CustomRotaryControl : juce::Slider
{
    CustomRotaryControl(juce::RangedAudioParameter& rap, const juce::String& title);

    void                               paint(juce::Graphics& g) override;
    [[nodiscard]] juce::Rectangle<int> getRotaryBounds() const;

private:
    juce::RangedAudioParameter* param;
};
