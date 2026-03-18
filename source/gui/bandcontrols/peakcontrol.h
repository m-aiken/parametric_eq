#pragma once

#include <JuceHeader.h>

#include "CustomRotaryControl.h"
#include "PowerButton.h"

class PeakControl final : public juce::Component
{
public:
    PeakControl(juce::AudioProcessorValueTreeState& apvts, const int _bandNum);
    void paint(juce::Graphics& g) override;
    void resized() override;

    void setNodeIsActive(bool activeState);
    void setEnablement();

private:
    int  bandNum;
    bool nodeIsActive { false };

    PowerButton                                          powerButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment buttonAttachment;

    CustomRotaryControl                                  freqSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment freqAttachment;

    CustomRotaryControl                                  gainSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment;

    CustomRotaryControl                                  qSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment qAttachment;
};
