#pragma once

#include <JuceHeader.h>

#include "EqBandWidget.h"

class EqControls final
    : public juce::Component
    , public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit EqControls(juce::AudioProcessorValueTreeState& apvts);
    ~EqControls() override;

    void resized() override;
    void parameterChanged(const juce::String& parameter_id, float new_value) override;

private:
    void setActiveBand(const ParametricEq::FILTER_ID& active_band);

    juce::AudioProcessorValueTreeState& apvts_;

    EqBandWidget low_cut_controls_;
    EqBandWidget low_shelf_controls_;
    EqBandWidget peak_band_1_;
    EqBandWidget peak_band_2_;
    EqBandWidget peak_band_3_;
    EqBandWidget peak_band_4_;
    EqBandWidget peak_band_5_;
    EqBandWidget high_shelf_controls_;
    EqBandWidget high_cut_controls_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqControls)
};
