#include "GraphicEqEditor.h"
#include "GraphicEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphicEqEditor::GraphicEqEditor(GraphicEqProcessor& p)
    : AudioProcessorEditor(&p)
    , processor_ref_(p)
    , analyzer_container_(processor_ref_.getApvts(),
                          processor_ref_.getSampleRate(),
                          processor_ref_.preEqMonoFifoL,
                          processor_ref_.preEqMonoFifoR,
                          processor_ref_.postEqMonoFifoL,
                          processor_ref_.postEqMonoFifoR)
    , eq_controls_(processor_ref_.getApvts())
    , active_node_(analyzer_container_.getActiveNodeIndex())
{
    setLookAndFeel(&lnf_);

    addAndMakeVisible(analyzer_container_);
    addAndMakeVisible(eq_controls_);

    setSize(800, 600);

    startTimerHz(20);
}

/*---------------------------------------------------------------------------
**
*/
GraphicEqEditor::~GraphicEqEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqEditor::paint(juce::Graphics& g)
{
    g.fillAll(Theme::MAIN_BACKGROUND);
    g.setFont(Theme::getFont());
    g.setColour(Theme::TEXT);

    const auto& analyzer_bounds = analyzer_container_.getBounds();

    const auto left_db_scale_bounds =
        juce::Rectangle< int >(0, analyzer_bounds.getY(), analyzer_bounds.getX(), analyzer_bounds.getHeight());
    const auto right_db_scale_bounds = juce::Rectangle< int >(analyzer_bounds.getRight(),
                                                              analyzer_bounds.getY(),
                                                              analyzer_bounds.getX(),
                                                              analyzer_bounds.getHeight());

    drawDbLabels(g, left_db_scale_bounds);
    drawDbLabels(g, right_db_scale_bounds);

    const auto text_height            = g.getCurrentFont().getHeight();
    const auto frequency_scale_bounds = juce::Rectangle< int >(analyzer_bounds.getX(),
                                                               analyzer_bounds.getY() - text_height,
                                                               analyzer_bounds.getWidth(),
                                                               text_height);

    drawFrequencyLabels(g, frequency_scale_bounds);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqEditor::resized()
{
    using Track = juce::Grid::TrackInfo;
    using Fr    = juce::Grid::Fr;

    juce::Grid grid;

    grid.autoColumns = Track(Fr(100));

    grid.templateRows = {
        Track(Fr(70)),  //! Analyser.
        Track(Fr(30)),  //! Band controls.
    };

    grid.items.add(juce::GridItem(analyzer_container_));
    grid.items.add(juce::GridItem(eq_controls_));

    const auto& full_bounds = getLocalBounds();
    const auto& padding     = Theme::getFont().getHeight();
    grid.performLayout(full_bounds.withSizeKeepingCentre(full_bounds.getWidth() - (padding * 6),
                                                         full_bounds.getHeight() - (padding * 4)));
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqEditor::timerCallback()
{
    if (active_node_ != analyzer_container_.getActiveNodeIndex()) {
        active_node_ = analyzer_container_.getActiveNodeIndex();
        eq_controls_.setBandHasNodeSelection(active_node_);
    }
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ void
GraphicEqEditor::drawDbLabels(juce::Graphics& g, const juce::Rectangle< int >& label_bounds)
{
    const auto& text_height = g.getCurrentFont().getHeight();

    const auto db_scale_y      = label_bounds.getY();
    const auto db_scale_height = label_bounds.getHeight();
    const auto db_scale_width  = label_bounds.getWidth();
    const auto db_scale_x      = label_bounds.getX();

    for (auto db = GraphicEq::MIN_DB; db <= GraphicEq::MAX_DB; db += 12) {
        const auto y_coord  = juce::jmap< int >(db, GraphicEq::MIN_DB, GraphicEq::MAX_DB, db_scale_height, 0);
        const auto text_y   = y_coord + db_scale_y - (text_height * 0.5);
        auto       dbString = juce::String(db);

        g.drawFittedText((db > 0 ? '+' + dbString : dbString),  // text
                         db_scale_x,                            // x
                         text_y,                                // y
                         db_scale_width,                        // width
                         text_height,                           // height
                         juce::Justification::centred,          // justification
                         1);                                    // max num lines
    }
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ void
GraphicEqEditor::drawFrequencyLabels(juce::Graphics& g, const juce::Rectangle< int >& label_bounds)
{
    static const std::vector< float > freqs { 20.f, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, 20000.f };

    const auto bounds_x     = label_bounds.getX();
    const auto bounds_y     = label_bounds.getY();
    const auto bounds_width = label_bounds.getWidth();

    const auto text_height = g.getCurrentFont().getHeight();
    const auto text_width  = text_height * 2.5;

    for (auto i = 1; i < freqs.size() - 1; ++i) {
        const auto normalized_x = juce::mapFromLog10< float >(freqs[i], GraphicEq::MIN_HZ, GraphicEq::MAX_HZ);
        const auto label_x      = bounds_x + bounds_width * normalized_x;

        juce::String text = freqs[i] >= 1000.f ? juce::String(freqs[i] / 1000.f) + "kHz" : juce::String(freqs[i]) + "Hz";

        g.drawFittedText(text,                                      // text
                         label_x - (text_width * 0.5),              // x
                         bounds_y - (text_height * 0.5),            // y
                         text_width,                                // width
                         text_height,                               // height
                         juce::Justification::horizontallyCentred,  // justification
                         1);                                        // max num lines
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
