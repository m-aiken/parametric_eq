#include "SliderTextBox.h"
#include "TextOnlyRotaryLnf.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
void
TextOnlyRotaryLnf::drawRotarySlider(juce::Graphics& g,
                                    int             x,
                                    int             y,
                                    int             width,
                                    int             height,
                                    float           slider_pos_proportional,
                                    float           rotary_start_angle,
                                    float           rotary_end_angle,
                                    juce::Slider&   slider)
{
    juce::ignoreUnused(g, x, y, width, height, slider_pos_proportional, rotary_start_angle, rotary_end_angle, slider);

    // We only override drawRotarySlider to explicitly draw nothing.
}

/*---------------------------------------------------------------------------
**
*/
juce::Label*
TextOnlyRotaryLnf::createSliderTextBox(juce::Slider& slider)
{
    juce::ignoreUnused(slider);

    return new SliderTextBox();
}

/*---------------------------------------------------------------------------
**
*/
juce::Slider::SliderLayout
TextOnlyRotaryLnf::getSliderLayout(juce::Slider& slider)
{
    juce::Slider::SliderLayout layout;

    const auto& full_bounds = slider.getLocalBounds();
    const auto& font_height = static_cast< int >(ParametricEq::Theme::getFont().getHeight());

    layout.sliderBounds  = full_bounds;
    layout.textBoxBounds = full_bounds.withSizeKeepingCentre(full_bounds.getWidth(), font_height);

    return layout;
}

/*---------------------------------------------------------------------------
** End of File
*/
