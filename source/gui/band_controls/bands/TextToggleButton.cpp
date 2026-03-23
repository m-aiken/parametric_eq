#include "GraphicEqGlobals.h"
#include "TextToggleButton.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
TextToggleButton::TextToggleButton(juce::AudioProcessorValueTreeState& apvts,
                                   const juce::String&                 parameter_id,
                                   const juce::String&                 label_text)
    : button_attachment_(apvts, parameter_id, *this)
{
    setButtonText(label_text);
}

/*---------------------------------------------------------------------------
**
*/
void
TextToggleButton::paintButton(juce::Graphics& g, bool should_draw_button_as_highlighted, bool should_draw_button_as_down)
{
    juce::ignoreUnused(should_draw_button_as_highlighted, should_draw_button_as_down);

    const bool on = getToggleState();
    g.setFont(Theme::getFont());
    g.setColour(Theme::TEXT.withAlpha(on ? GraphicEq::WIDGET_ALPHA_FULL : GraphicEq::WIDGET_ALPHA_DIM));
    g.drawFittedText(getButtonText(), getLocalBounds(), juce::Justification::centred, 1);
}

/*---------------------------------------------------------------------------
**
*/
void
TextToggleButton::mouseEnter(const juce::MouseEvent& e)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

/*---------------------------------------------------------------------------
** End of File
*/
