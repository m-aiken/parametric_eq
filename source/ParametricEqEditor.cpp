#include "ParametricEqEditor.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
ParametricEqEditor::ParametricEqEditor(ParametricEqProcessor& p)
    : AudioProcessorEditor(&p)
    , processor_ref_(p)
    , graph_(p)
    , eq_controls_(processor_ref_.getApvts())
{
    addAndMakeVisible(graph_);
    addAndMakeVisible(eq_controls_);

    setSize(1024, 600);
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqEditor::paint(juce::Graphics& g)
{
    g.fillAll(Theme::MAIN_BACKGROUND);
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqEditor::resized()
{
    using Track = juce::Grid::TrackInfo;
    using Fr    = juce::Grid::Fr;

    juce::Grid grid;

    grid.autoColumns = Track(Fr(100));

    grid.templateRows = {
        Track(Fr(80)),  //! Graph.
        Track(Fr(20)),  //! Band controls.
    };

    grid.items.add(juce::GridItem(graph_));
    grid.items.add(juce::GridItem(eq_controls_));

    grid.performLayout(getLocalBounds());
}

/*---------------------------------------------------------------------------
** End of File
*/
