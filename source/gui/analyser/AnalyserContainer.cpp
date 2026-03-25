#include "AnalyserContainer.h"
#include "Theme.h"

/*static*/ const std::vector< float > AnalyserContainer::HZ_LIST = {
    20.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, 20000.f,
};

/*---------------------------------------------------------------------------
**
*/
AnalyserContainer::AnalyserContainer(juce::AudioProcessorValueTreeState& apvts,
                                     const double                        sample_rate,
                                     MonoBufferFifo&                     pre_eq_buffer_l,
                                     MonoBufferFifo&                     pre_eq_buffer_r,
                                     MonoBufferFifo&                     post_eq_buffer_l,
                                     MonoBufferFifo&                     post_eq_buffer_r)
    : waveform_pre_eq_(sample_rate, pre_eq_buffer_l, pre_eq_buffer_r, Theme::ANALYSER_PRE_EQ)
    , waveform_post_eq_(sample_rate, post_eq_buffer_l, post_eq_buffer_r, Theme::ANALYSER_POST_EQ)
    , response_curve_(apvts, sample_rate)
{
    addAndMakeVisible(background_grid_);
    addAndMakeVisible(waveform_pre_eq_);
    addAndMakeVisible(waveform_post_eq_);
    addAndMakeVisible(response_curve_);
}

/*---------------------------------------------------------------------------
**
*/
void
AnalyserContainer::paint(juce::Graphics& g)
{
    const auto& bounds = getLocalBounds();

    g.setFont(Theme::getFont());
    g.setColour(Theme::ANALYSER_TEXT);

    drawDbLabels(g, { Theme::UI_PADDING, 0, TEXT_WIDTH, bounds.getHeight() });
    drawFrequencyLabels(g, { 0, Theme::UI_PADDING, bounds.getWidth(), static_cast< int >(Theme::getFont().getHeight()) });
}

/*---------------------------------------------------------------------------
**
*/
void
AnalyserContainer::resized()
{
    const auto& bounds = getLocalBounds();

    background_grid_.setBounds(bounds);
    waveform_pre_eq_.setBounds(bounds);
    waveform_post_eq_.setBounds(bounds);
    response_curve_.setBounds(bounds);
}

/*---------------------------------------------------------------------------
**
*/
size_t
AnalyserContainer::getActiveNodeIndex()
{
    return response_curve_.getActiveNodeIndex();
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ void
AnalyserContainer::drawDbLabels(const juce::Graphics& g, const juce::Rectangle< int >& bounds)
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
AnalyserContainer::drawFrequencyLabels(const juce::Graphics& g, const juce::Rectangle< int >& bounds)
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
