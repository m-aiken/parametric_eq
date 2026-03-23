#include "EqBandBase.h"
#include "GraphicEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
/*static*/ void
EqBandBase::initialiseLabel(juce::Label& label)
{
    label.setFont(Theme::getFont());
    label.setColour(juce::Label::ColourIds::textColourId, Theme::TEXT);
    label.setJustificationType(juce::Justification::centred);
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ juce::String
EqBandBase::getFrequencyString(const CustomRotarySlider& slider)
{
    double       hz                 = slider.getValue();
    int          num_decimal_places = 0;
    juce::String suffix             = " Hz";

    if (hz >= 1000.0) {
        hz                 = hz / 1000.0;
        num_decimal_places = 2;
        suffix             = " kHz";
    }

    return juce::String(hz, num_decimal_places) + suffix;
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ juce::Rectangle< int >
EqBandBase::getEnablementButtonBounds() const
{
    const auto& full_bounds = getLocalBounds();

    const auto x      = full_bounds.getX();
    const auto y      = full_bounds.getY();
    const auto width  = full_bounds.getWidth();
    const auto height = static_cast< int >(full_bounds.getHeight() * (ENABLEMENT_BUTTON / 100.0));

    const auto initial_rect = juce::Rectangle< int >(x, y, width, height);

    // We want a rectangle reduces by some amount of padding.
    constexpr auto padding = (Theme::UI_PADDING * 2);
    return initial_rect.withSizeKeepingCentre(initial_rect.getWidth() - padding, initial_rect.getHeight() - padding);
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ juce::Rectangle< int >
EqBandBase::getControlSectionBounds() const
{
    const auto& full_bounds = getLocalBounds();

    constexpr auto y_offset = (ENABLEMENT_BUTTON / 100.0);

    const auto x      = full_bounds.getX();
    const auto y      = static_cast< int >(full_bounds.getHeight() * y_offset);
    const auto width  = full_bounds.getWidth();
    const auto height = static_cast< int >(full_bounds.getHeight() * (CONTROL_SECTION / 100.0));

    const auto initial_rect = juce::Rectangle< int >(x, y, width, height);

    // We want a square that's centered in the initial rectangle.
    // Apply some reduction factor for padding.
    constexpr auto reduction_factor = 0.9;
    const auto     min_diameter     = static_cast< int >(std::min(width, height) * reduction_factor);

    return initial_rect.withSizeKeepingCentre(min_diameter, min_diameter);
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ juce::Rectangle< int >
EqBandBase::getLabel1Bounds() const
{
    const auto& full_bounds = getLocalBounds();

    constexpr auto y_offset = (ENABLEMENT_BUTTON / 100.0) + (CONTROL_SECTION / 100.0);

    const auto x      = full_bounds.getX();
    const auto y      = static_cast< int >(full_bounds.getHeight() * y_offset);
    const auto width  = full_bounds.getWidth();
    const auto height = static_cast< int >(full_bounds.getHeight() * (LABEL_1 / 100.0));

    return { x, y, width, height };
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ juce::Rectangle< int >
EqBandBase::getLabel2Bounds() const
{
    const auto& full_bounds = getLocalBounds();

    constexpr auto y_offset = (ENABLEMENT_BUTTON / 100.0) + (CONTROL_SECTION / 100.0) + (LABEL_1 / 100.0);

    const auto x      = full_bounds.getX();
    const auto y      = static_cast< int >(full_bounds.getHeight() * y_offset);
    const auto width  = full_bounds.getWidth();
    const auto height = static_cast< int >(full_bounds.getHeight() * (ENABLEMENT_BUTTON / 100.0));

    return { x, y, width, height };
}

/*---------------------------------------------------------------------------
** End of File
*/
