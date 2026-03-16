#pragma once

#include <JuceHeader.h>

#include "CustomRotaryControl.h"
#include "PowerButton.h"

struct PeakControl : juce::Component {
    PeakControl(juce::AudioProcessorValueTreeState& apvts, const int _bandNum);
    void paint(juce::Graphics& g) override;
    void resized() override;

    void setNodeIsActive(bool activeState);
    void setEnablement();

private:
    int  bandNum;
    bool nodeIsActive { false };

    std::unique_ptr< PowerButton >                                          powerButton;
    std::unique_ptr< juce::AudioProcessorValueTreeState::ButtonAttachment > buttonAttachment;

    std::unique_ptr< CustomRotaryControl >                                  freqSlider;
    std::unique_ptr< CustomRotaryControl >                                  gainSlider;
    std::unique_ptr< CustomRotaryControl >                                  qSlider;
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > freqAttachment;
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > gainAttachment;
    std::unique_ptr< juce::AudioProcessorValueTreeState::SliderAttachment > qAttachment;
};
