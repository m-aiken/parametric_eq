#include "EqBandBase.h"
#include "GraphicEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
EqBandBase::EqBandBase(juce::AudioProcessorValueTreeState& apvts, const GraphicEq::FILTER_ID& filter_id)
    : band_enablement_button_(apvts, filter_id)
    , apvts_(apvts)
    , filter_id_(filter_id)
    , is_last_active_band_(false)
{
    band_enablement_button_.onClick = [this]() { setSubWidgetEnablement(); };

    addAndMakeVisible(band_enablement_button_);

    const auto& [enabled_, frequency_, gain_, q_, slope_] = GraphicEq::PARAMETER_MAP.at(filter_id_);

    if (frequency_ != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        apvts_.getParameter(frequency_)->addListener(this);
    }

    if (gain_ != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        apvts_.getParameter(gain_)->addListener(this);
    }

    if (q_ != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        apvts_.getParameter(q_)->addListener(this);
    }

    if (slope_ != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        apvts_.getParameter(slope_)->addListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
EqBandBase::~EqBandBase()
{
    const auto& [enabled_, frequency_, gain_, q_, slope_] = GraphicEq::PARAMETER_MAP.at(filter_id_);

    if (frequency_ != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        apvts_.getParameter(frequency_)->removeListener(this);
    }

    if (gain_ != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        apvts_.getParameter(gain_)->removeListener(this);
    }

    if (q_ != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        apvts_.getParameter(q_)->removeListener(this);
    }

    if (slope_ != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        apvts_.getParameter(slope_)->removeListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
EqBandBase::paint(juce::Graphics& g)
{
    g.setColour(is_last_active_band_ ? Theme::ACTIVE_BAND_BACKGROUND : Theme::MAIN_BACKGROUND);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), Theme::BORDER_RADIUS);
}

/*---------------------------------------------------------------------------
**
*/
void
EqBandBase::parameterValueChanged(int parameter_index, float new_value)
{
    juce::ignoreUnused(parameter_index, new_value);

    juce::MessageManagerLock mml;
    claimLastActive();
}

/*---------------------------------------------------------------------------
**
*/
void
EqBandBase::parameterGestureChanged(int parameter_index, bool gesture_is_starting)
{
    juce::ignoreUnused(parameter_index, gesture_is_starting);

    // Do nothing. Only implemented because it's a pure virtual method of juce::AudioProcessorParameter::Listener.
}

/*---------------------------------------------------------------------------
**
*/
void
EqBandBase::setIsLastActiveBand(const bool is_last_active)
{
    is_last_active_band_ = is_last_active;

    repaint();
}

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
/*static*/ juce::String
EqBandBase::getGainString(const CustomRotarySlider& slider)
{
    const auto&   db_value           = slider.getValue();
    const auto&   prefix             = db_value > 0 ? "+" : "";
    constexpr int num_decimal_places = 2;

    return prefix + juce::String(db_value, num_decimal_places) + " dB";
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
