#include "GraphBackground.h"
#include "GraphicEqGlobals.h"
#include "Theme.h"

/*static*/ const std::vector< float > GraphBackground::HZ_LIST = {
    20.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, 20000.f,
};

/*---------------------------------------------------------------------------
**
*/
GraphBackground::GraphBackground()
{
    setBufferedToImage(true);
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

    g.setColour(Theme::ANALYSER_GRID.withAlpha(0.2f));

    // dB grid lines.
    for (auto i = static_cast< int >(GraphicEq::MIN_DB); i <= static_cast< int >(GraphicEq::MAX_DB); i += DB_INTERVAL) {
        const auto y =
            juce::jmap< float >(static_cast< float >(i), GraphicEq::MIN_DB, GraphicEq::MAX_DB, full_height_f, 0.f);
        g.drawLine(2, y, full_width_f - 2, y, 1.f);
    }

    // Hz grid lines.
    for (auto i = static_cast< int >(GraphicEq::MIN_HZ) + 1; i < static_cast< int >(GraphicEq::MAX_HZ) - 1; ++i) {
        if (shouldDrawFrequency(i)) {
            const auto raw_x = juce::mapFromLog10< float >(static_cast< float >(i), GraphicEq::MIN_HZ, GraphicEq::MAX_HZ);
            const auto x_coord = static_cast< int >(full_width_f * raw_x);

            g.setColour(Theme::ANALYSER_GRID.withAlpha(getAlphaForFrequency(i)));
            g.drawVerticalLine(x_coord, 0.f, full_height_f);
        }
    }

    const auto& bounds_i      = getLocalBounds();
    const auto& full_width_i  = bounds_i.getWidth();
    const auto& full_height_i = bounds_i.getHeight();

    g.setFont(Theme::getFont());
    g.setColour(Theme::ANALYSER_TEXT);

    // dB labels.
    drawDbLabels(g, { Theme::UI_PADDING, 0, TEXT_WIDTH, full_height_i });

    // Hz labels.
    drawFrequencyLabels(g, { 0, Theme::UI_PADDING, full_width_i, static_cast< int >(Theme::getFont().getHeight()) });
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
**
*/
/*static*/ float
GraphBackground::getAlphaForFrequency(const int hz)
{
    if (hz == 50 || hz == 100 || hz == 200 || hz == 500 || hz == 1000 || hz == 2000 || hz == 5000 || hz == 10000) {
        return 0.2f;
    }

    return 0.07f;
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ void
GraphBackground::drawDbLabels(const juce::Graphics& g, const juce::Rectangle< int >& bounds)
{
    const auto& text_height   = static_cast< int >(g.getCurrentFont().getHeight());
    const auto& bounds_width  = bounds.getWidth();
    const auto& bounds_height = bounds.getHeight();
    const auto& bounds_x      = bounds.getX();

    for (auto db = static_cast< int >(GraphicEq::MIN_DB); db <= static_cast< int >(GraphicEq::MAX_DB); db += DB_INTERVAL) {
        const auto raw_y     = juce::jmap< int >(db, GraphicEq::MIN_DB, GraphicEq::MAX_DB, bounds_height, 0);
        const auto shifted_y = raw_y - text_height - Theme::UI_PADDING;
        const auto text      = (db > 0 ? "+" : "") + juce::String(db) + " dB";

        g.drawFittedText(text, bounds_x, shifted_y, bounds_width, text_height, juce::Justification::centredLeft, 1);
    }
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ void
GraphBackground::drawFrequencyLabels(const juce::Graphics& g, const juce::Rectangle< int >& bounds)
{
    const auto& text_height  = static_cast< int >(Theme::getFont().getHeight());
    const auto& bounds_y     = bounds.getY();
    const auto& bounds_width = bounds.getWidth();

    for (auto i = 1; i < HZ_LIST.size() - 1; ++i) {
        auto       hz        = HZ_LIST.at(i);
        const auto raw_x     = juce::mapFromLog10< float >(hz, GraphicEq::MIN_HZ, GraphicEq::MAX_HZ);
        const auto shifted_x = static_cast< int >(bounds_width * raw_x) + Theme::UI_PADDING;

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
** End of File
*/
