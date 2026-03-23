#include "CustomRotarySlider.h"
#include "GraphicEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
CustomRotarySlider::CustomRotarySlider(juce::AudioProcessorValueTreeState& apvts,
                                       const juce::String&                 parameter_id,
                                       const juce::String&                 title)
    : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , slider_attachment_(apvts, parameter_id, *this)
    , param(apvts.getParameter(parameter_id))
{
    setName(title);
}

/*---------------------------------------------------------------------------
**
*/
void
CustomRotarySlider::paint(juce::Graphics& g)
{
    if (param == nullptr) {
        return;
    }

    auto startAngle = juce::degreesToRadians(180.f + 45.f);
    auto endAngle   = juce::degreesToRadians(180.f - 45.f) + juce::MathConstants< float >::twoPi;

    auto range = param->getNormalisableRange();

    g.setFont(Theme::getFont());
    g.setColour(Theme::TEXT);

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

    r.setSize(g.getCurrentFont().getStringWidth(getName()), Theme::getFont().getHeight());
    r.setCentre(center);
    g.drawFittedText(getName(), r.toNearestInt(), juce::Justification::centred, 1);
}

/*---------------------------------------------------------------------------
**
*/
juce::Rectangle< int >
CustomRotarySlider::getRotaryBounds() const
{
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= Theme::getFont().getHeight() * 0.6;

    juce::Rectangle< int > r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(bounds.getCentreY() - (size * 0.5));

    return r;
}

/*---------------------------------------------------------------------------
**
*/
void
CustomRotarySlider::setEnablement(const bool enable)
{
    setEnabled(enable);
    setAlpha(enable ? Theme::ENABLED_ALPHA : Theme::DISABLED_ALPHA);
}

/*---------------------------------------------------------------------------
** End of File
*/
