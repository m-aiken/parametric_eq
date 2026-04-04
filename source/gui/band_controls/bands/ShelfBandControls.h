#pragma once

#include <JuceHeader.h>

#include "EqBandBase.h"
#include "GraphicEqGlobals.h"

class ShelfBandControls final : public EqBandBase
{
public:
    ShelfBandControls(juce::AudioProcessorValueTreeState& apvts, const GraphicEq::FILTER_ID& filter_id);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void setFreqLabelText();
    void setGainLabelText();
    void setSubWidgetEnablement() override;

    CustomRotarySlider freq_slider_;
    CustomRotarySlider gain_slider_;
    juce::Label        freq_value_label_;
    juce::Label        gain_value_label_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShelfBandControls)
};
