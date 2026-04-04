#include "GraphicEqGlobals.h"
#include "TextToggleButton.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
TextToggleButton::TextToggleButton(juce::AudioProcessorValueTreeState& apvts, const GraphicEq::FILTER_ID& filter_id)
    : button_attachment_(apvts, GraphicEq::PARAMETER_MAP.at(filter_id).enabled_, *this)
    , background_colour_(Theme::TEXT_BUTTON_BG_STD)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
    setButtonText(GraphicEq::getFilterButtonLabel(filter_id));
}

/*---------------------------------------------------------------------------
**
*/
void
TextToggleButton::paintButton(juce::Graphics& g, bool should_draw_button_as_highlighted, bool should_draw_button_as_down)
{
    juce::ignoreUnused(should_draw_button_as_highlighted, should_draw_button_as_down);

    g.setColour(background_colour_);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), Theme::BORDER_RADIUS);

    const bool on = getToggleState();
    g.setFont(Theme::getFont());
    g.setColour(Theme::TEXT.withAlpha(on ? Theme::ENABLED_ALPHA : Theme::DISABLED_ALPHA));
    g.drawFittedText(getButtonText(), getLocalBounds(), juce::Justification::centred, 1);
}

/*---------------------------------------------------------------------------
**
*/
void
TextToggleButton::mouseEnter(const juce::MouseEvent& event)
{
    juce::ToggleButton::mouseEnter(event);

    background_colour_ = Theme::TEXT_BUTTON_BG_HOVER;
    repaint();
}

/*---------------------------------------------------------------------------
**
*/
void
TextToggleButton::mouseExit(const MouseEvent& event)
{
    juce::ToggleButton::mouseExit(event);

    background_colour_ = Theme::TEXT_BUTTON_BG_STD;
    repaint();
}

/*---------------------------------------------------------------------------
** End of File
*/
