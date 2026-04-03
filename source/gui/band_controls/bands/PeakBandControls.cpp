#include "GraphicEqGlobals.h"
#include "PeakBandControls.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
PeakBandControls::PeakBandControls(juce::AudioProcessorValueTreeState& apvts, const int band_num)
    : node_is_active_(false)
    , band_enablement_button_(apvts,
                              getPeakParamName(GraphicEq::PEAK_ENABLED, band_num),
                              "BAND " + juce::String(band_num + 1))
    , freq_slider_(apvts, getPeakParamName(GraphicEq::PEAK_FREQ, band_num), "Hz")
    , gain_slider_(apvts, getPeakParamName(GraphicEq::PEAK_GAIN, band_num), "dB")
    , q_slider_(apvts, getPeakParamName(GraphicEq::PEAK_Q, band_num), "Q")
{
    band_enablement_button_.onClick = [this]() { setSubWidgetEnablement(); };

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

    addAndMakeVisible(band_enablement_button_);
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
PeakBandControls::paint(juce::Graphics& g)
{
    g.setColour(node_is_active_ ? Theme::PEAK_BAND_FOCUS : Theme::MAIN_BACKGROUND);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), Theme::BORDER_RADIUS);
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
PeakBandControls::setNodeIsActive(bool activeState)
{
    node_is_active_ = activeState;
    repaint();
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
    const auto& db_value = gain_slider_.getValue();
    const auto& prefix   = db_value > 0 ? "+" : "";

    gain_value_label_.setText(prefix + juce::String(db_value) + " dB", juce::sendNotification);
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
