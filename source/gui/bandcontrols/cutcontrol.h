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

    PowerButton                                          powerButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment buttonAttachment;

    CustomRotaryControl                                  freqSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment freqAttachment;

    CustomLinearSlider                                   slopeSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment slopeAttachment;
};