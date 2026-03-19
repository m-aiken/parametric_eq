#pragma once

#include <JuceHeader.h>

class PowerButton final : public juce::ToggleButton
{
public:
    void paint(juce::Graphics& g) override;
    void mouseEnter(const juce::MouseEvent& e) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerButton)
};
