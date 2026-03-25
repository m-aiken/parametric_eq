#include "GraphicEqEditor.h"
#include "GraphicEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphicEqEditor::GraphicEqEditor(GraphicEqProcessor& p)
    : AudioProcessorEditor(&p)
    , processor_ref_(p)
    , analyser_(processor_ref_.getApvts(),
                processor_ref_.getSampleRate(),
                processor_ref_.preEqMonoFifoL,
                processor_ref_.preEqMonoFifoR,
                processor_ref_.postEqMonoFifoL,
                processor_ref_.postEqMonoFifoR)
    , eq_controls_(processor_ref_.getApvts())
    , active_node_(analyser_.getActiveNodeIndex())
{
    setLookAndFeel(&lnf_);

    addAndMakeVisible(analyser_);
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

    grid.items.add(juce::GridItem(analyser_));
    grid.items.add(juce::GridItem(eq_controls_));

#if 0
    const auto& full_bounds = getLocalBounds();
    const auto& padding     = Theme::getFont().getHeight();
    grid.performLayout(full_bounds.withSizeKeepingCentre(full_bounds.getWidth() - (padding * 6),
                                                         full_bounds.getHeight() - (padding * 4)));
#endif
    grid.performLayout(getLocalBounds());
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqEditor::timerCallback()
{
    if (active_node_ != analyser_.getActiveNodeIndex()) {
        active_node_ = analyser_.getActiveNodeIndex();
        eq_controls_.setBandHasNodeSelection(active_node_);
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
