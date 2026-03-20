#include "ColourPalette.h"
#include "GraphicEqGlobals.h"
#include "TextToggleButton.h"

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
    const bool on = getToggleState();
    g.setFont(GraphicEq::getFont());
    g.setColour(ColourPalette::Text.withAlpha(on ? GraphicEq::WIDGET_ALPHA_FULL : GraphicEq::WIDGET_ALPHA_DIM));
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
