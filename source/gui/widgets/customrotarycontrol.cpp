#include "ColourPalette.h"
#include "CustomRotaryControl.h"
#include "GraphicEqGlobals.h"

/*---------------------------------------------------------------------------
**
*/
CustomRotaryControl::CustomRotaryControl(juce::RangedAudioParameter& rap, const juce::String& title)
    : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , param(&rap)
{
    setName(title);
}

/*---------------------------------------------------------------------------
**
*/
void
CustomRotaryControl::paint(juce::Graphics& g)
{
    if (param == nullptr) {
        return;
    }

    auto startAngle = juce::degreesToRadians(180.f + 45.f);
    auto endAngle   = juce::degreesToRadians(180.f - 45.f) + juce::MathConstants< float >::twoPi;

    auto range = param->getNormalisableRange();

    g.setFont(GraphicEqGlobals::getFont());
    g.setColour(ColourPalette::RotaryText);

    auto valueToDraw = juce::jmap< float >(getValue(), range.start, range.end, startAngle, endAngle);

    auto rotaryBounds = getRotaryBounds();

    getLookAndFeel().drawRotarySlider(g,
                                      rotaryBounds.getX(),       // x
                                      rotaryBounds.getY(),       // y
                                      rotaryBounds.getWidth(),   // width
                                      rotaryBounds.getHeight(),  // height
                                      valueToDraw,               // position
                                      startAngle,                // start angle
                                      endAngle,                  // end angle
                                      *this);                    // slider

    auto center = rotaryBounds.toFloat().getCentre();

    juce::Rectangle< float > r;

    r.setSize(g.getCurrentFont().getStringWidth(getName()), GraphicEqGlobals::getFont().getHeight());
    r.setCentre(center);
    g.drawFittedText(getName(), r.toNearestInt(), juce::Justification::centred, 1);
}

/*---------------------------------------------------------------------------
**
*/
juce::Rectangle< int >
CustomRotaryControl::getRotaryBounds() const
{
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= GraphicEqGlobals::getFont().getHeight() * 0.6;

    juce::Rectangle< int > r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(bounds.getCentreY() - (size * 0.5));

    return r;
}

/*---------------------------------------------------------------------------
** End of File
*/
