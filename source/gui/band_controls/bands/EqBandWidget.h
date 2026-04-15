#pragma once

#include <JuceHeader.h>

#include "ParametricEqGlobals.h"
#include "TextOnlyRotary.h"
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

    void setIsActiveBand(const bool is_active);

private:
    void setSubWidgetEnablement() const;

    juce::AudioProcessorValueTreeState& apvts_;
    const ParametricEq::FILTER_ID       filter_id_;
    bool                                is_active_band_;

    TextToggleButton                  band_enablement_button_;
    std::unique_ptr< TextOnlyRotary > slider_1_;
    std::unique_ptr< TextOnlyRotary > slider_2_;
    std::unique_ptr< TextOnlyRotary > slider_3_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqBandWidget)
};
