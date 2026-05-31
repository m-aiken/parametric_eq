#include "FilterToggleButton.h"
#include "ParametricEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
FilterToggleButton::FilterToggleButton(ParametricEqProcessor& processor_ref, const ParametricEq::FILTER_ID& filter_id)
    : processor_ref_(processor_ref)
    , filter_id_(filter_id)
    , button_attachment_(processor_ref.getApvts(), ParametricEq::PARAMETER_MAP.at(filter_id).enabled_, *this)
    , background_colour_(Theme::TEXT_BUTTON_BG_STD)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);

    processor_ref_.getApvts().addParameterListener(ParametricEq::PLUGIN_BYPASS_PARAMETER, this);
}

/*---------------------------------------------------------------------------
**
*/
FilterToggleButton::~FilterToggleButton()
{
    processor_ref_.getApvts().removeParameterListener(ParametricEq::PLUGIN_BYPASS_PARAMETER, this);
}

/*---------------------------------------------------------------------------
**
*/
void
FilterToggleButton::paintButton(juce::Graphics& g, bool should_draw_button_as_highlighted, bool should_draw_button_as_down)
{
    juce::ignoreUnused(should_draw_button_as_highlighted, should_draw_button_as_down);

    const auto& bounds      = getLocalBounds().toFloat();
    const auto  icon_width  = bounds.getWidth() * 0.2f;
    const auto  icon_height = bounds.getHeight() * 0.35f;
    const auto  icon_bounds = bounds.withSizeKeepingCentre(icon_width, icon_height);
    const auto  left        = icon_bounds.getX();
    const auto  right       = icon_bounds.getRight();
    const auto  top         = icon_bounds.getY();
    const auto  bottom      = icon_bounds.getBottom();
    const auto  centre_y    = icon_bounds.getCentreY();

    // Background.
    g.setColour(background_colour_);
    g.fillRoundedRectangle(bounds, Theme::BORDER_RADIUS);

    // Icon.
    g.setColour(processor_ref_.pluginIsEnabled() && getToggleState() ? Theme::TEXT : Theme::TEXT_DISABLED);

    switch (filter_id_) {
    case ParametricEq::LOW_CUT:
    {
        const auto knee = (icon_width * 0.33f);

        const juce::Point p0(left + HALF_LINE_THICKNESS, bottom - HALF_LINE_THICKNESS);
        const juce::Point p1(left + knee + HALF_LINE_THICKNESS, top + HALF_LINE_THICKNESS);
        const juce::Point p2(right - HALF_LINE_THICKNESS, top + HALF_LINE_THICKNESS);

        juce::Path path;
        path.startNewSubPath(p0);
        path.lineTo(p1);
        path.lineTo(p2);
        g.strokePath(path.createPathWithRoundedCorners(CORNER_RADIUS), juce::PathStrokeType(LINE_THICKNESS));

        break;
    }

    case ParametricEq::LOW_SHELF:
    {
        const auto knee_1 = (icon_width * 0.45f);
        const auto knee_2 = (icon_width * 0.65f);

        {
            const juce::Point p0(left + HALF_LINE_THICKNESS, top + HALF_LINE_THICKNESS);
            const juce::Point p1(left + knee_1, top + HALF_LINE_THICKNESS);
            const juce::Point p2(left + knee_2, centre_y);
            const juce::Point p3(right - HALF_LINE_THICKNESS, centre_y);

            juce::Path path;
            path.startNewSubPath(p0);
            path.lineTo(p1);
            path.lineTo(p2);
            path.lineTo(p3);
            g.strokePath(path.createPathWithRoundedCorners(CORNER_RADIUS), juce::PathStrokeType(LINE_THICKNESS));
        }

        {
            const juce::Point p0(left + HALF_LINE_THICKNESS, bottom - HALF_LINE_THICKNESS);
            const juce::Point p1(left + knee_1, bottom - HALF_LINE_THICKNESS);
            const juce::Point p2(left + knee_2, centre_y);
            const juce::Point p3(right - HALF_LINE_THICKNESS, centre_y);

            juce::Path path;
            path.startNewSubPath(p0);
            path.lineTo(p1);
            path.lineTo(p2);
            path.lineTo(p3);
            g.strokePath(path.createPathWithRoundedCorners(CORNER_RADIUS), juce::PathStrokeType(LINE_THICKNESS));
        }

        break;
    }

    case ParametricEq::PEAK_BAND_1:
    case ParametricEq::PEAK_BAND_2:
    case ParametricEq::PEAK_BAND_3:
    case ParametricEq::PEAK_BAND_4:
    case ParametricEq::PEAK_BAND_5:
    {
        const auto knee_1 = (icon_width * 0.25f);
        const auto knee_2 = (icon_width * 0.40f);
        const auto knee_3 = (icon_width * 0.60f);
        const auto knee_4 = (icon_width * 0.75f);

        {
            const juce::Point p0(left + HALF_LINE_THICKNESS, centre_y);
            const juce::Point p1(left + knee_1 + HALF_LINE_THICKNESS, centre_y);
            const juce::Point p2(left + knee_2, top + HALF_LINE_THICKNESS);
            const juce::Point p3(left + knee_3, top + HALF_LINE_THICKNESS);
            const juce::Point p4(left + knee_4 - HALF_LINE_THICKNESS, centre_y);
            const juce::Point p5(right - HALF_LINE_THICKNESS, centre_y);

            juce::Path path;
            path.startNewSubPath(p0);
            path.lineTo(p1);
            path.lineTo(p2);
            path.lineTo(p3);
            path.lineTo(p4);
            path.lineTo(p5);
            g.strokePath(path.createPathWithRoundedCorners(CORNER_RADIUS), juce::PathStrokeType(LINE_THICKNESS));
        }

        {
            const juce::Point p0(left + HALF_LINE_THICKNESS, centre_y);
            const juce::Point p1(left + knee_1 + HALF_LINE_THICKNESS, centre_y);
            const juce::Point p2(left + knee_2, bottom - HALF_LINE_THICKNESS);
            const juce::Point p3(left + knee_3, bottom - HALF_LINE_THICKNESS);
            const juce::Point p4(left + knee_4 - HALF_LINE_THICKNESS, centre_y);
            const juce::Point p5(right - HALF_LINE_THICKNESS, centre_y);

            juce::Path path;
            path.startNewSubPath(p0);
            path.lineTo(p1);
            path.lineTo(p2);
            path.lineTo(p3);
            path.lineTo(p4);
            path.lineTo(p5);
            g.strokePath(path.createPathWithRoundedCorners(CORNER_RADIUS), juce::PathStrokeType(LINE_THICKNESS));
        }

        break;
    }

    case ParametricEq::HIGH_SHELF:
    {
        const auto knee_1 = (icon_width * 0.35f);
        const auto knee_2 = (icon_width * 0.55f);

        {
            const juce::Point p0(left + HALF_LINE_THICKNESS, centre_y);
            const juce::Point p1(left + knee_1, centre_y);
            const juce::Point p2(left + knee_2, top + HALF_LINE_THICKNESS);
            const juce::Point p3(right - HALF_LINE_THICKNESS, top + HALF_LINE_THICKNESS);

            juce::Path path;
            path.startNewSubPath(p0);
            path.lineTo(p1);
            path.lineTo(p2);
            path.lineTo(p3);
            g.strokePath(path.createPathWithRoundedCorners(CORNER_RADIUS), juce::PathStrokeType(LINE_THICKNESS));
        }

        {
            const juce::Point p0(left + HALF_LINE_THICKNESS, centre_y);
            const juce::Point p1(left + knee_1, centre_y);
            const juce::Point p2(left + knee_2, bottom - HALF_LINE_THICKNESS);
            const juce::Point p3(right - HALF_LINE_THICKNESS, bottom - HALF_LINE_THICKNESS);

            juce::Path path;
            path.startNewSubPath(p0);
            path.lineTo(p1);
            path.lineTo(p2);
            path.lineTo(p3);
            g.strokePath(path.createPathWithRoundedCorners(CORNER_RADIUS), juce::PathStrokeType(LINE_THICKNESS));
        }

        break;
    }

    case ParametricEq::HIGH_CUT:
    {
        const auto knee = (icon_width * 0.66f);

        const juce::Point p0(right - HALF_LINE_THICKNESS, bottom - HALF_LINE_THICKNESS);
        const juce::Point p1(left + knee - HALF_LINE_THICKNESS, top + HALF_LINE_THICKNESS);
        const juce::Point p2(left + HALF_LINE_THICKNESS, top + HALF_LINE_THICKNESS);

        juce::Path path;
        path.startNewSubPath(p0);
        path.lineTo(p1);
        path.lineTo(p2);
        g.strokePath(path.createPathWithRoundedCorners(CORNER_RADIUS), juce::PathStrokeType(LINE_THICKNESS));

        break;
    }

    case ParametricEq::NULL_BAND:
    default:
        break;
    }
}

/*---------------------------------------------------------------------------
**
*/
void
FilterToggleButton::mouseEnter(const juce::MouseEvent& event)
{
    juce::ToggleButton::mouseEnter(event);

    if (!processor_ref_.pluginIsEnabled()) {
        return;
    }

    background_colour_ = Theme::TEXT_BUTTON_BG_HOVER;
    repaint();
}

/*---------------------------------------------------------------------------
**
*/
void
FilterToggleButton::mouseExit(const MouseEvent& event)
{
    juce::ToggleButton::mouseExit(event);

    background_colour_ = Theme::TEXT_BUTTON_BG_STD;
    repaint();
}

/*---------------------------------------------------------------------------
**
*/
void
FilterToggleButton::parameterChanged(const juce::String& parameter_id, float new_value)
{
    juce::ignoreUnused(new_value);

    if (parameter_id == ParametricEq::PLUGIN_BYPASS_PARAMETER) {
        setMouseCursor(processor_ref_.pluginIsEnabled() ? juce::MouseCursor::PointingHandCursor :
                                                          juce::MouseCursor::NormalCursor);
        repaint();
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
