#pragma once

#include <JuceHeader.h>

#include "EqBandBase.h"
#include "GraphicEqGlobals.h"

class PeakBandControls final : public EqBandBase
{
public:
    PeakBandControls(juce::AudioProcessorValueTreeState& apvts, const GraphicEq::FILTER_ID& filter_id);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setNodeIsActive(bool activeState);

private:
    void setFreqLabelText();
    void setGainLabelText();
    void setSubWidgetEnablement() override;

    bool node_is_active_;

    CustomRotarySlider freq_slider_;
    CustomRotarySlider gain_slider_;
    CustomRotarySlider q_slider_;
    juce::Label        freq_value_label_;
    juce::Label        gain_value_label_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PeakBandControls)
};
