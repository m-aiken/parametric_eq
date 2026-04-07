#pragma once

#include <JuceHeader.h>

#include "CustomRotarySlider.h"
#include "GraphicEqGlobals.h"
#include "TextToggleButton.h"

class EqBandBase
    : public juce::Component
    , public juce::AudioProcessorParameter::Listener
{
public:
    EqBandBase(juce::AudioProcessorValueTreeState& apvts, const GraphicEq::FILTER_ID& filter_id);
    ~EqBandBase() override;

    void paint(juce::Graphics& g) override;
    void parameterValueChanged(int parameter_index, float new_value) override;
    void parameterGestureChanged(int parameter_index, bool gesture_is_starting) override;

    void setIsLastActiveBand(const bool is_last_active);

protected:
    virtual void setSubWidgetEnablement() = 0;

    static void         initialiseLabel(juce::Label& label);
    static juce::String getFrequencyString(const CustomRotarySlider& slider);

    std::function< void() > claimLastActive;

    enum SUB_WIDGET_PROPORTIONS {
        ENABLEMENT_BUTTON = 20,
        CONTROL_SECTION   = 50,
        LABEL_1           = 15,
        LABEL_2           = 15,
    };

    juce::Rectangle< int > getEnablementButtonBounds() const;
    juce::Rectangle< int > getControlSectionBounds() const;
    juce::Rectangle< int > getLabel1Bounds() const;
    juce::Rectangle< int > getLabel2Bounds() const;

    TextToggleButton band_enablement_button_;

    juce::AudioProcessorValueTreeState& apvts_;
    const GraphicEq::FILTER_ID          filter_id_;

    bool is_last_active_band_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqBandBase)
};
