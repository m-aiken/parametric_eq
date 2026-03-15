#pragma once

#include <JuceHeader.h>

//==============================================================================
struct ThemeButton : juce::ToggleButton
{
    ThemeButton();
    void paint(juce::Graphics& g) override;
    void mouseEnter(const juce::MouseEvent& e) override;

private:
    std::unique_ptr<DrawableImage> icon;
};
