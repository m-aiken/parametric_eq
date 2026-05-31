#pragma once

#include <JuceHeader.h>

#include "FilterToggleButton.h"
#include "ParametricEqGlobals.h"
#include "ParametricEqProcessor.h"
#include "TextOnlyRotary.h"

class EqBandWidget final
    : public juce::Component
    , public juce::AudioProcessorValueTreeState::Listener
{
public:
    enum BAND_TYPE {
        CUT_BAND,
        SHELF_BAND,
        PEAK_BAND,
    };

public:
    EqBandWidget(ParametricEqProcessor&         processor_ref,
                 const ParametricEq::FILTER_ID& filter_id,
                 const BAND_TYPE&               band_type);
    ~EqBandWidget() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void parameterChanged(const juce::String& parameter_id, float new_value) override;

    void setIsActiveBand(const bool is_active);

private:
    void setSubWidgetEnablement();

    ParametricEqProcessor&        processor_ref_;
    const ParametricEq::FILTER_ID filter_id_;
    bool                          is_active_band_;

    FilterToggleButton                band_enablement_button_;
    std::unique_ptr< TextOnlyRotary > slider_1_;
    std::unique_ptr< TextOnlyRotary > slider_2_;
    std::unique_ptr< TextOnlyRotary > slider_3_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqBandWidget)
};
