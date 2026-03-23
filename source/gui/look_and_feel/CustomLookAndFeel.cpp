#include "CustomLookAndFeel.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
void
CustomLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                    int             x,
                                    int             y,
                                    int             width,
                                    int             height,
                                    float           slider_pos_proportional,
                                    float           rotary_start_angle,
                                    float           rotary_end_angle,
                                    juce::Slider&   slider)
{
    juce::ignoreUnused(rotary_start_angle, rotary_end_angle, slider);

    const auto& bounds = juce::Rectangle< float >(x, y, width, height);
    const auto& centre = bounds.getCentre();

    g.setColour(Theme::ROTARY_FILL);
    g.fillEllipse(bounds);
    g.setColour(Theme::ROTARY_THUMB);

    juce::Path p;

    juce::Rectangle< float > r;
    r.setLeft(centre.getX() - 2);
    r.setRight(centre.getX() + 2);
    r.setTop(bounds.getY() + 4);
    r.setBottom(bounds.getY() + 8);

    p.addRoundedRectangle(r, 2.f);

    const auto& transform = juce::AffineTransform().rotated(slider_pos_proportional, centre.getX(), centre.getY());

    p.applyTransform(transform);

    g.fillPath(p);
}

/*---------------------------------------------------------------------------
** End of File
*/
