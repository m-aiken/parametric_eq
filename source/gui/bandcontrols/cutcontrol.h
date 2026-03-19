#pragma once

#include <JuceHeader.h>

#include "FilterUtils.h"
#include "CustomLinearSlider.h"
#include "CustomRotaryControl.h"
#include "PowerButton.h"

class CutControl final : public juce::Component
{
public:
    CutControl(juce::AudioProcessorValueTreeState& apvts, const FilterUtils::ChainPositions& _chainPosition);
    void paint(juce::Graphics& g) override;
    void resized() override;

    void setEnablement();

private:
    FilterUtils::ChainPositions chainPosition;

    std::unique_ptr< PowerButton >         powerButton;
    std::unique_ptr< CustomRotaryControl > freqSlider;
    std::unique_ptr< CustomLinearSlider >  slopeSlider;

    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    std::unique_ptr< ButtonAttachment >                          powerAttachment;
    std::unique_ptr< SliderAttachment >                          freqAttachment;
    std::unique_ptr< SliderAttachment >                          slopeAttachment;
};