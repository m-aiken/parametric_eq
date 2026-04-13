#include "TextOnlyRotary.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
TextOnlyRotary::TextOnlyRotary(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameter_id)
    : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , slider_attachment_(apvts, parameter_id, *this)
    , param_(apvts.getParameter(parameter_id))
{
}

/*---------------------------------------------------------------------------
**
*/
void
TextOnlyRotary::paint(juce::Graphics& g)
{
    if (param_ != nullptr) {
        g.setFont(Theme::getFont());
        g.setColour(Theme::TEXT);
        g.drawFittedText(param_->getCurrentValueAsText(), getLocalBounds(), juce::Justification::centred, 1);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
TextOnlyRotary::setEnablement(const bool enable)
{
    setEnabled(enable);
    setAlpha(enable ? Theme::ENABLED_ALPHA : Theme::DISABLED_ALPHA);
}

/*---------------------------------------------------------------------------
** End of File
*/
