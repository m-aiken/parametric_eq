#pragma once

#include <JuceHeader.h>

#include "EqBandBase.h"
#include "GraphicEqGlobals.h"

class CutBandControls final : public EqBandBase
{
public:
    CutBandControls(juce::AudioProcessorValueTreeState& apvts,
                    const GraphicEq::FILTER_ID&         filter_id,
                    std::function< void() >&&           callback);

    void resized() override;

private:
    void setFreqLabelText();
    void setSlopeLabelText();
    void setSubWidgetEnablement() override;

    CustomRotarySlider freq_slider_;
    CustomRotarySlider slope_slider_;
    juce::Label        freq_value_label_;
    juce::Label        slope_value_label_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutBandControls)
};