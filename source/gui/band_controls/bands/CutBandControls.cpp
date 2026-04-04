#include "CutBandControls.h"
#include "GraphicEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
CutBandControls::CutBandControls(juce::AudioProcessorValueTreeState& apvts, const GraphicEq::FILTER_ID& filter_id)
    : EqBandBase(apvts, filter_id)
    , freq_slider_(apvts, GraphicEq::PARAMETER_MAP.at(filter_id).frequency_, "Hz")
    , slope_slider_(apvts, GraphicEq::PARAMETER_MAP.at(filter_id).slope_, "dB")
{
    freq_slider_.onValueChange = [this]() {
        setFreqLabelText();
        repaint();
    };

    slope_slider_.onValueChange = [this]() {
        setSlopeLabelText();
        repaint();
    };

    addAndMakeVisible(freq_slider_);
    addAndMakeVisible(slope_slider_);
    addAndMakeVisible(freq_value_label_);
    addAndMakeVisible(slope_value_label_);

    freq_value_label_.setFont(Theme::getFont());
    freq_value_label_.setColour(juce::Label::ColourIds::textColourId, Theme::TEXT);
    freq_value_label_.setJustificationType(juce::Justification::centred);
    slope_value_label_.setFont(Theme::getFont());
    slope_value_label_.setColour(juce::Label::ColourIds::textColourId, Theme::TEXT);
    slope_value_label_.setJustificationType(juce::Justification::centred);

    setFreqLabelText();
    setSlopeLabelText();
    setSubWidgetEnablement();
}

/*---------------------------------------------------------------------------
**
*/
void
CutBandControls::resized()
{
    band_enablement_button_.setBounds(getEnablementButtonBounds());

    const auto& rotary_section  = getControlSectionBounds();
    const auto& rotary_diameter = static_cast< int >(rotary_section.getWidth() * 0.5);

    freq_slider_.setBounds(rotary_section.getCentreX() - static_cast< int >(rotary_diameter * 0.5),
                           rotary_section.getY(),
                           rotary_diameter,
                           rotary_diameter);

    slope_slider_.setBounds(rotary_section.getCentreX() - static_cast< int >(rotary_diameter * 0.5),
                            rotary_section.getBottom() - rotary_diameter,
                            rotary_diameter,
                            rotary_diameter);

    freq_value_label_.setBounds(getLabel1Bounds());
    slope_value_label_.setBounds(getLabel2Bounds());
}

/*---------------------------------------------------------------------------
**
*/
void
CutBandControls::setFreqLabelText()
{
    freq_value_label_.setText(getFrequencyString(freq_slider_), juce::sendNotification);
}

/*---------------------------------------------------------------------------
**
*/
void
CutBandControls::setSlopeLabelText()
{
    const int slope = (static_cast< int >(slope_slider_.getValue()) + 1) * 12;
    slope_value_label_.setText(juce::String(slope) + " db/Oct", juce::sendNotification);
}

/*---------------------------------------------------------------------------
**
*/
void
CutBandControls::setSubWidgetEnablement()
{
    const bool enabled = band_enablement_button_.getToggleState();
    freq_slider_.setEnablement(enabled);
    slope_slider_.setEnablement(enabled);
    freq_value_label_.setEnabled(enabled);
    slope_value_label_.setEnabled(enabled);
}

/*---------------------------------------------------------------------------
** End of File
*/
