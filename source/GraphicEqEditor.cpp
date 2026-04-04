#include "GraphicEqEditor.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphicEqEditor::GraphicEqEditor(GraphicEqProcessor& p)
    : AudioProcessorEditor(&p)
    , processor_ref_(p)
    , graph_(p)
    , eq_controls_(processor_ref_.getApvts())
    , active_node_(graph_.getActiveNodeIndex())
{
    setLookAndFeel(&lnf_);

    addAndMakeVisible(graph_);
    addAndMakeVisible(eq_controls_);

    setSize(1024, 600);

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
        Track(Fr(70)),  //! Graph.
        Track(Fr(30)),  //! Band controls.
    };

    grid.items.add(juce::GridItem(graph_));
    grid.items.add(juce::GridItem(eq_controls_));

    grid.performLayout(getLocalBounds());
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqEditor::timerCallback()
{
    if (active_node_ != graph_.getActiveNodeIndex()) {
        active_node_ = graph_.getActiveNodeIndex();
        eq_controls_.setBandHasNodeSelection(active_node_);
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
