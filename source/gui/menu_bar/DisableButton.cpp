#include "DisableButton.h"
#include "ParametricEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
DisableButton::DisableButton(ParametricEqProcessor& processor_ref)
    : processor_ref_(processor_ref)
    , button_attachment_(processor_ref.getApvts(), parametric_eq::PLUGIN_BYPASS_PARAMETER, *this)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

/*---------------------------------------------------------------------------
**
*/
void
DisableButton::paintButton(juce::Graphics& g, bool should_draw_button_as_highlighted, bool should_draw_button_as_down)
{
    juce::ignoreUnused(should_draw_button_as_highlighted, should_draw_button_as_down);

    const auto& bounds        = getLocalBounds().toFloat();
    const auto  base_diameter = bounds.getHeight();
    const auto  icon_diameter = base_diameter / 2.f;
    const auto  left          = juce::Rectangle(0.f, 0.f, base_diameter, base_diameter);
    const auto  icon_bounds   = left.withSizeKeepingCentre(icon_diameter, icon_diameter);
    const auto  right         = juce::Rectangle(bounds.getRight() - base_diameter, 0.f, base_diameter, base_diameter);
    const auto  orb_bounds    = right.withSizeKeepingCentre(ORB_DIAMETER, ORB_DIAMETER);

    // Background.
    g.fillAll(parametric_eq::theme::MAIN_BACKGROUND);

    // Power icon: Outer ring.
    g.setColour(parametric_eq::theme::POWER_ICON);
    g.drawEllipse(icon_bounds, ICON_LINE_THICKNESS);

    // Power icon: Split in outer ring.
    g.setColour(parametric_eq::theme::MAIN_BACKGROUND);
    g.fillRect(icon_bounds.getCentreX() - (ICON_LINE_THICKNESS * 2.f),
               icon_bounds.getY() - ICON_LINE_THICKNESS,
               ICON_LINE_THICKNESS * 4.f,
               ICON_LINE_THICKNESS * 2.f);

    // Power icon: Thumb.
    g.setColour(parametric_eq::theme::POWER_ICON);
    g.fillRect(icon_bounds.getCentreX() - (ICON_LINE_THICKNESS / 2.f),
               icon_bounds.getY() - 1.f,
               ICON_LINE_THICKNESS,
               icon_diameter / 2.f);

    // Orb.
    g.setColour(processor_ref_.pluginIsEnabled() ? parametric_eq::theme::POWER_ORB_ON :
                                                   parametric_eq::theme::POWER_ORB_OFF);
    g.fillEllipse(orb_bounds);
}

/*---------------------------------------------------------------------------
** End of File
*/
