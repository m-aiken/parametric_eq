#pragma once

#include <JuceHeader.h>

#include "CustomRotarySlider.h"
#include "ParametricEqGlobals.h"
#include "TextToggleButton.h"

class EqBandWidget final : public juce::Component
{
public:
    enum BAND_TYPE {
        CUT_BAND,
        SHELF_BAND,
        PEAK_BAND,
    };

public:
    EqBandWidget(juce::AudioProcessorValueTreeState& apvts,
                 const ParametricEq::FILTER_ID&      filter_id,
                 const BAND_TYPE&                    band_type);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setIsLastActiveBand(const bool is_last_active);

private:
    void setSubWidgetEnablement() const;

    juce::AudioProcessorValueTreeState& apvts_;
    const ParametricEq::FILTER_ID       filter_id_;
    bool                                is_last_active_band_;

    TextToggleButton                      band_enablement_button_;
    std::unique_ptr< CustomRotarySlider > slider_1_;
    std::unique_ptr< CustomRotarySlider > slider_2_;
    std::unique_ptr< CustomRotarySlider > slider_3_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqBandWidget)
};
