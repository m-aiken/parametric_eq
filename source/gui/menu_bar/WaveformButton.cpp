#include "ParametricEqGlobals.h"
#include "ParametricEqTheme.h"
#include "WaveformButton.h"

/*---------------------------------------------------------------------------
**
*/
WaveformButton::WaveformButton(ParametricEqProcessor& processor_ref)
    : processor_ref_(processor_ref)
    , button_attachment_(processor_ref.getApvts(), parametric_eq::SHOW_WAVEFORM_PARAMETER, *this)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
    setBufferedToImage(true);
}

/*---------------------------------------------------------------------------
**
*/
void
WaveformButton::paintButton(juce::Graphics& g, bool should_draw_button_as_highlighted, bool should_draw_button_as_down)
{
    juce::ignoreUnused(should_draw_button_as_highlighted, should_draw_button_as_down);

    const auto& bounds        = getLocalBounds().toFloat();
    const auto  path_height   = bounds.getHeight() / 2.f;
    const auto  path_width    = bounds.getWidth() - path_height;
    const auto& path_bounds   = bounds.withSizeKeepingCentre(path_width, path_height);
    const auto  x_origin      = path_bounds.getX();
    const auto  centre_y      = path_bounds.getCentreY();
    const auto  top           = path_bounds.getY();
    const auto  bottom        = path_bounds.getBottom();
    const auto  corner_radius = path_width * 0.25f;

    // Draw a sine wave.
    const juce::Point p0(x_origin, centre_y);
    const juce::Point p1(x_origin + (path_width * 0.25f), top);
    const juce::Point p2(x_origin + (path_width * 0.5f), centre_y);
    const juce::Point p3(x_origin + (path_width * 0.75f), bottom);
    const juce::Point p4(x_origin + path_width, centre_y);

    juce::Path path;
    path.startNewSubPath(p0);
    path.lineTo(p1);
    path.lineTo(p2);
    path.lineTo(p3);
    path.lineTo(p4);

    g.setColour(processor_ref_.showWaveform() ? parametric_eq::theme::WAVEFORM_BUTTON_ON :
                                                parametric_eq::theme::WAVEFORM_BUTTON_OFF);

    g.strokePath(path.createPathWithRoundedCorners(corner_radius), juce::PathStrokeType(ICON_LINE_THICKNESS));
}

/*---------------------------------------------------------------------------
** End of File
*/
