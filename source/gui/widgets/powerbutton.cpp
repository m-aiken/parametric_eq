#include "colourpalette.h"
#include "powerbutton.h"

//==============================================================================
void PowerButton::paint(juce::Graphics& g)
{
    auto  bounds        = getLocalBounds();
    float lineThickness = 2.f;

    g.setColour(getToggleState() ? ColourPalette::getColourV2(ColourPalette::PowerButtonOn) : ColourPalette::getColourV2(ColourPalette::PowerButtonOff));
    g.drawEllipse(bounds.toFloat().reduced(lineThickness), lineThickness);

    g.setColour(ColourPalette::getColourV2(ColourPalette::MainBackground));
    g.fillRect(bounds.getCentreX() - lineThickness, 0.f, lineThickness * 2, bounds.getHeight() * 0.5f);

    g.setColour(getToggleState() ? ColourPalette::getColourV2(ColourPalette::PowerButtonOn) : ColourPalette::getColourV2(ColourPalette::PowerButtonOff));
    g.fillRect(bounds.getCentreX() - (lineThickness * 0.5), lineThickness * 0.5, lineThickness, bounds.getHeight() * 0.4f);
}

void PowerButton::mouseEnter(const juce::MouseEvent& e)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}
