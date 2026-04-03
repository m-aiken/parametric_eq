#include "CutBandControls.h"
#include "GraphicEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
CutBandControls::CutBandControls(juce::AudioProcessorValueTreeState&  apvts,
                                 const FilterFactory::ChainPositions& chain_position)
{
    const bool  low_cut    = chain_position == FilterFactory::ChainPositions::LowCut;
    const auto& freq_id    = low_cut ? GraphicEq::LOW_CUT_FREQ.id_ : GraphicEq::HIGH_CUT_FREQ.id_;
    const auto& slope_id   = low_cut ? GraphicEq::LOW_CUT_SLOPE.id_ : GraphicEq::HIGH_CUT_SLOPE.id_;
    const auto& enabled_id = low_cut ? GraphicEq::LOW_CUT_ENABLED.id_ : GraphicEq::HIGH_CUT_ENABLED.id_;

    band_enablement_button_ = std::make_unique< TextToggleButton >(apvts, enabled_id, low_cut ? "LOW CUT" : "HIGH CUT");
    freq_slider_            = std::make_unique< CustomRotarySlider >(apvts, freq_id, "Hz");
    slope_slider_           = std::make_unique< CustomRotarySlider >(apvts, slope_id, "dB");

    band_enablement_button_->onClick = [this]() { setSubWidgetEnablement(); };

    freq_slider_->onValueChange = [this]() {
        setFreqLabelText();
        repaint();
    };

    slope_slider_->onValueChange = [this]() {
        setSlopeLabelText();
        repaint();
    };

    addAndMakeVisible(*band_enablement_button_);
    addAndMakeVisible(*freq_slider_);
    addAndMakeVisible(*slope_slider_);
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
    band_enablement_button_->setBounds(getEnablementButtonBounds());

    const auto& rotary_section  = getControlSectionBounds();
    const auto& rotary_diameter = static_cast< int >(rotary_section.getWidth() * 0.5);

    freq_slider_->setBounds(rotary_section.getCentreX() - static_cast< int >(rotary_diameter * 0.5),
                            rotary_section.getY(),
                            rotary_diameter,
                            rotary_diameter);

    slope_slider_->setBounds(rotary_section.getCentreX() - static_cast< int >(rotary_diameter * 0.5),
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
    freq_value_label_.setText(getFrequencyString(*freq_slider_), juce::sendNotification);
}

/*---------------------------------------------------------------------------
**
*/
void
CutBandControls::setSlopeLabelText()
{
    const int slope = (static_cast< int >(slope_slider_->getValue()) + 1) * 12;
    slope_value_label_.setText(juce::String(slope) + " db/Oct", juce::sendNotification);
}

/*---------------------------------------------------------------------------
**
*/
void
CutBandControls::setSubWidgetEnablement()
{
    const bool enabled = band_enablement_button_->getToggleState();
    freq_slider_->setEnablement(enabled);
    slope_slider_->setEnablement(enabled);
    freq_value_label_.setEnabled(enabled);
    slope_value_label_.setEnabled(enabled);
}

/*---------------------------------------------------------------------------
** End of File
*/
