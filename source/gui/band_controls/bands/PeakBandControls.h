#pragma once

#include <JuceHeader.h>

#include "EqBandBase.h"
#include "ParametricEqGlobals.h"

class PeakBandControls final : public EqBandBase
{
public:
    PeakBandControls(juce::AudioProcessorValueTreeState& apvts, const ParametricEq::FILTER_ID& filter_id);

    void resized() override;

private:
    void setFreqLabelText();
    void setGainLabelText();
    void setSubWidgetEnablement() override;

    CustomRotarySlider freq_slider_;
    CustomRotarySlider gain_slider_;
    CustomRotarySlider q_slider_;
    juce::Label        freq_value_label_;
    juce::Label        gain_value_label_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PeakBandControls)
};
