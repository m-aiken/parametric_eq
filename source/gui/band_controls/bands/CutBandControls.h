#pragma once

#include <JuceHeader.h>

#include "EqBandBase.h"
#include "FilterUtils.h"
#include "TextToggleButton.h"

class CutBandControls final : public EqBandBase
{
public:
    CutBandControls(juce::AudioProcessorValueTreeState& apvts, const FilterUtils::ChainPositions& chain_position);

    void resized() override;

private:
    void setFreqLabelText();
    void setSlopeLabelText();
    void setSubWidgetEnablement();

    std::unique_ptr< TextToggleButton >   band_enablement_button_;
    std::unique_ptr< CustomRotarySlider > freq_slider_;
    std::unique_ptr< CustomRotarySlider > slope_slider_;
    juce::Label                           freq_value_label_;
    juce::Label                           slope_value_label_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutBandControls)
};