#pragma once

#include <JuceHeader.h>

class CustomLinearSlider final : public juce::Slider
{
public:
    explicit CustomLinearSlider(juce::RangedAudioParameter* rap);

    void paint(juce::Graphics& g) override;

    void setEnablement(const bool enable);

private:
    juce::RangedAudioParameter* param;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLinearSlider)
};
