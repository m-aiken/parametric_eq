#include "GraphBackground.h"
#include "ParametricEqGlobals.h"
#include "Theme.h"

/*static*/ const std::vector< float > GraphBackground::HZ_LIST = {
    20.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, 20000.f,
};

/*---------------------------------------------------------------------------
**
*/
GraphBackground::GraphBackground(ParametricEqProcessor& processor_ref)
    : processor_ref_(processor_ref)
{
    setBufferedToImage(true);

    processor_ref_.getApvts().addParameterListener(ParametricEq::PLUGIN_BYPASS_PARAMETER, this);
}

/*---------------------------------------------------------------------------
**
*/
GraphBackground::~GraphBackground()
{
    processor_ref_.getApvts().removeParameterListener(ParametricEq::PLUGIN_BYPASS_PARAMETER, this);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphBackground::paint(juce::Graphics& g)
{
    const auto& bounds_f      = getLocalBounds().toFloat();
    const auto& full_width_f  = bounds_f.getWidth();
    const auto& full_height_f = bounds_f.getHeight();

    // dB grid lines.
    for (auto i = static_cast< int >(ParametricEq::MIN_DB); i <= static_cast< int >(ParametricEq::MAX_DB); ++i) {
        const auto y =
            juce::jmap< float >(static_cast< float >(i), ParametricEq::MIN_DB, ParametricEq::MAX_DB, full_height_f, 0.f);

        g.setColour(ParametricEq::Theme::ANALYSER_GRID.withAlpha(getAlphaForDb(i)));
        g.drawLine(2, y, full_width_f - 2, y, 1.f);
    }

    // Hz grid lines.
    for (auto i = static_cast< int >(ParametricEq::MIN_HZ) + 1; i < static_cast< int >(ParametricEq::MAX_HZ) - 1; ++i) {
        if (shouldDrawFrequency(i)) {
            const auto raw_x   = juce::mapFromLog10< float >(static_cast< float >(i),
                                                           ParametricEq::MIN_HZ,
                                                           ParametricEq::MAX_HZ);
            const auto x_coord = static_cast< int >(full_width_f * raw_x);

            g.setColour(ParametricEq::Theme::ANALYSER_GRID.withAlpha(getAlphaForFrequency(i)));
            g.drawVerticalLine(x_coord, 0.f, full_height_f);
        }
    }

    g.setFont(ParametricEq::Theme::getFont());
    g.setColour(processor_ref_.pluginIsEnabled() ? ParametricEq::Theme::ANALYSER_TEXT :
                                                   ParametricEq::Theme::PLUGIN_DISABLED_ANALYSER_TEXT);

    // dB labels.
    drawDbLabels(g);

    // Hz labels.
    drawFrequencyLabels(g);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphBackground::parameterChanged(const juce::String& parameter_id, float new_value)
{
    juce::ignoreUnused(new_value);

    if (parameter_id == ParametricEq::PLUGIN_BYPASS_PARAMETER) {
        repaint();
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphBackground::drawDbLabels(const juce::Graphics& g) const
{
    const auto& text_height   = static_cast< int >(g.getCurrentFont().getHeight());
    const auto& bounds_height = getLocalBounds().getHeight();

    for (auto i = static_cast< int >(ParametricEq::MIN_DB) + 3; i <= static_cast< int >(ParametricEq::MAX_DB) - 3; ++i) {
        if (i % DB_LABEL_INTERVAL != 0) {
            continue;
        }

        const auto raw_y     = juce::jmap< int >(i,
                                             static_cast< int >(ParametricEq::MIN_DB),
                                             static_cast< int >(ParametricEq::MAX_DB),
                                             bounds_height,
                                             0);
        const auto shifted_y = raw_y - text_height - ParametricEq::Theme::UI_PADDING;
        const auto text      = (i > 0 ? "+" : "") + juce::String(i) + " dB";

        g.drawFittedText(text,
                         ParametricEq::Theme::UI_PADDING,
                         shifted_y,
                         TEXT_WIDTH,
                         text_height,
                         juce::Justification::centredLeft,
                         1);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphBackground::drawFrequencyLabels(const juce::Graphics& g) const
{
    const auto& text_height  = static_cast< int >(ParametricEq::Theme::getFont().getHeight());
    const auto& bounds_y     = ParametricEq::Theme::UI_PADDING;
    const auto& bounds_width = getLocalBounds().toFloat().getWidth();

    for (size_t i = 1; i < HZ_LIST.size() - 1; ++i) {
        auto       hz        = HZ_LIST.at(i);
        const auto raw_x     = juce::mapFromLog10< float >(hz, ParametricEq::MIN_HZ, ParametricEq::MAX_HZ);
        const auto shifted_x = static_cast< int >(bounds_width * raw_x) + ParametricEq::Theme::UI_PADDING;

        juce::String suffix = " Hz";

        if (hz >= 1000.f) {
            hz     = hz / 1000.f;
            suffix = " kHz";
        }

        const auto& text = juce::String(hz, 0) + suffix;

        g.drawFittedText(text, shifted_x, bounds_y, TEXT_WIDTH, text_height, juce::Justification::centredLeft, 1);
    }
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ float
GraphBackground::getAlphaForDb(const int db)
{
    if (db % DB_LABEL_INTERVAL == 0) {
        return LINE_ALPHA_BOLDER;
    }

    return LINE_ALPHA_LIGHTER;
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ float
GraphBackground::getAlphaForFrequency(const int hz)
{
    if (hz == 50 || hz == 100 || hz == 200 || hz == 500 || hz == 1000 || hz == 2000 || hz == 5000 || hz == 10000) {
        return LINE_ALPHA_BOLDER;
    }

    return LINE_ALPHA_LIGHTER;
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ bool
GraphBackground::shouldDrawFrequency(const int hz)
{
    bool should_draw = false;

    should_draw |= (hz <= 50 && hz % 10 == 0);
    should_draw |= (hz <= 200 && hz % 20 == 0);
    should_draw |= (hz <= 1000 && hz % 100 == 0);
    should_draw |= (hz <= 2000 && hz % 200 == 0);
    should_draw |= (hz <= 5000 && hz % 500 == 0);
    should_draw |= (hz % 1000 == 0);

    return should_draw;
}

/*---------------------------------------------------------------------------
** End of File
*/
