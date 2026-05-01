#include "TextOnlyRotary.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
TextOnlyRotary::TextOnlyRotary(juce::AudioProcessorValueTreeState& apvts, const juce::String& parameter_id)
    : juce::Slider(RotaryHorizontalVerticalDrag, TextBoxBelow)
    , slider_attachment_(apvts, parameter_id, *this)
    , param_(apvts.getParameter(parameter_id))
{
    hideTextBox(true);

    setLookAndFeel(&lnf_);
}

/*---------------------------------------------------------------------------
**
*/
TextOnlyRotary::~TextOnlyRotary()
{
    setLookAndFeel(nullptr);
}

/*---------------------------------------------------------------------------
**
*/
void
TextOnlyRotary::mouseDoubleClick(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);

    showTextBox();
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
