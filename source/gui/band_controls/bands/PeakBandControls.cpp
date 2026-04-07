#include "GraphicEqGlobals.h"
#include "PeakBandControls.h"

/*---------------------------------------------------------------------------
**
*/
PeakBandControls::PeakBandControls(juce::AudioProcessorValueTreeState& apvts,
                                   const GraphicEq::FILTER_ID&         filter_id,
                                   std::function< void() >&&           callback)
    : EqBandBase(apvts, filter_id)
    , freq_slider_(apvts, GraphicEq::PARAMETER_MAP.at(filter_id).frequency_, "Hz")
    , gain_slider_(apvts, GraphicEq::PARAMETER_MAP.at(filter_id).gain_, "dB")
    , q_slider_(apvts, GraphicEq::PARAMETER_MAP.at(filter_id).q_, "Q")
{
    claimLastActive = std::move(callback);

    freq_slider_.onValueChange = [this]() {
        setFreqLabelText();
        repaint();
    };

    gain_slider_.onValueChange = [this]() {
        setGainLabelText();
        repaint();
    };

    q_slider_.onValueChange = [this]() { repaint(); };

    initialiseLabel(freq_value_label_);
    initialiseLabel(gain_value_label_);

    addAndMakeVisible(freq_slider_);
    addAndMakeVisible(gain_slider_);
    addAndMakeVisible(q_slider_);
    addAndMakeVisible(freq_value_label_);
    addAndMakeVisible(gain_value_label_);

    setFreqLabelText();
    setGainLabelText();
    setSubWidgetEnablement();
}

/*---------------------------------------------------------------------------
**
*/
void
PeakBandControls::resized()
{
    band_enablement_button_.setBounds(getEnablementButtonBounds());

    const auto& rotary_section  = getControlSectionBounds();
    const auto& rotary_diameter = static_cast< int >(rotary_section.getWidth() * 0.5);

    freq_slider_.setBounds(rotary_section.getX(), rotary_section.getY(), rotary_diameter, rotary_diameter);
    gain_slider_.setBounds(rotary_section.getX(),
                           rotary_section.getBottom() - rotary_diameter,
                           rotary_diameter,
                           rotary_diameter);
    q_slider_.setBounds(rotary_section.getRight() - rotary_diameter,
                        rotary_section.getCentreY() - static_cast< int >(rotary_diameter * 0.5),
                        rotary_diameter,
                        rotary_diameter);

    freq_value_label_.setBounds(getLabel1Bounds());
    gain_value_label_.setBounds(getLabel2Bounds());
}

/*---------------------------------------------------------------------------
**
*/
void
PeakBandControls::setFreqLabelText()
{
    freq_value_label_.setText(getFrequencyString(freq_slider_), juce::sendNotification);
}

/*---------------------------------------------------------------------------
**
*/
void
PeakBandControls::setGainLabelText()
{
    gain_value_label_.setText(getGainString(gain_slider_), juce::sendNotification);
}

/*---------------------------------------------------------------------------
**
*/
void
PeakBandControls::setSubWidgetEnablement()
{
    const bool enabled = band_enablement_button_.getToggleState();
    freq_slider_.setEnablement(enabled);
    gain_slider_.setEnablement(enabled);
    q_slider_.setEnablement(enabled);
    freq_value_label_.setEnabled(enabled);
    gain_value_label_.setEnabled(enabled);
}

/*---------------------------------------------------------------------------
** End of File
*/
