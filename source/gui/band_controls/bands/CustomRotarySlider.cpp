#include "CustomRotarySlider.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
CustomRotarySlider::CustomRotarySlider(juce::AudioProcessorValueTreeState& apvts,
                                       const juce::String&                 parameter_id,
                                       const juce::String&                 label_text)
    : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , slider_attachment_(apvts, parameter_id, *this)
    , param(apvts.getParameter(parameter_id))
{
    setName(label_text);
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

    const auto& rotary_bounds = getRotaryBounds();

    g.setFont(Theme::getFont());
    g.setColour(Theme::TEXT);

    const auto& range         = param->getNormalisableRange();
    const auto  value_to_draw = juce::jmap< float >(getValue(), range.start, range.end, START_ANGLE, END_ANGLE);

    getLookAndFeel().drawRotarySlider(g,
                                      rotary_bounds.getX(),       //! x
                                      rotary_bounds.getY(),       //! y
                                      rotary_bounds.getWidth(),   //! width
                                      rotary_bounds.getHeight(),  //! height
                                      value_to_draw,              //! position
                                      START_ANGLE,                //! start angle
                                      END_ANGLE,                  //! end angle
                                      *this);                     //! slider

    g.drawFittedText(getName(), rotary_bounds, juce::Justification::centred, 1);
}

/*---------------------------------------------------------------------------
**
*/
juce::Rectangle< int >
CustomRotarySlider::getRotaryBounds() const
{
    const auto& bounds = getLocalBounds();
    auto        size   = juce::jmin(bounds.getWidth(), bounds.getHeight());

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
