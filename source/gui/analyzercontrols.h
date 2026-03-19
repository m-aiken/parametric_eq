#pragma once

#include <JuceHeader.h>

#include "CustomLinearSlider.h"

class AnalyzerControls final : public juce::Component
{
public:
    explicit AnalyzerControls(juce::AudioProcessorValueTreeState& apvts);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    CustomLinearSlider                                   fftOrderSlider;
    juce::AudioProcessorValueTreeState::SliderAttachment fftOrderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyzerControls)
};
