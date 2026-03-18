#include "ColourPalette.h"
#include "GraphicEqEditor.h"
#include "GraphicEqGlobals.h"

/*---------------------------------------------------------------------------
**
*/
GraphicEqEditor::GraphicEqEditor(GraphicEqProcessor& p)
    : AudioProcessorEditor(&p)
    , processor_ref_(p)
    , analyzer_controls_(processor_ref_.getApvts())
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

    addAndMakeVisible(analyzer_controls_);
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
    g.fillAll(ColourPalette::MainBackground);
    g.setFont(GraphicEq::FONT);
    g.setColour(ColourPalette::Text);

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
    const auto& bounds        = getLocalBounds();
    const auto& bounds_width  = bounds.getWidth();
    const auto& bounds_height = bounds.getHeight();
    const auto& padding       = GraphicEq::FONT.getHeight();

    const auto analyzer_bounds = juce::Rectangle< int >(padding * 3,
                                                        padding * 6,
                                                        bounds_width - (padding * 6),
                                                        static_cast< int >(
                                                            std::floor((bounds_height * 0.6) - (padding * 2))));

    const auto analyzer_width = analyzer_bounds.getWidth();

    analyzer_controls_.setBounds(bounds.getCentreX() - (analyzer_width * 0.25), padding, analyzer_width * 0.5, padding * 3);

    analyzer_container_.setBounds(analyzer_bounds);

    eq_controls_.setBounds(bounds.getCentreX() - (analyzer_width * 0.5),
                           analyzer_container_.getBottom() + padding,
                           analyzer_width,
                           bounds.getBottom() - analyzer_container_.getBottom() - (padding * 2));
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
