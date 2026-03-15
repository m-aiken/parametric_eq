#pragma once

#include <JuceHeader.h>

//==============================================================================
struct PowerButton : juce::ToggleButton
{
    void paint(juce::Graphics& g) override;
    void mouseEnter(const juce::MouseEvent& e) override;
};
