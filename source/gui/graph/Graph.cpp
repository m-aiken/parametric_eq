#include "Graph.h"
#include "ParametricEqTheme.h"

/*---------------------------------------------------------------------------
**
*/
Graph::Graph(ParametricEqProcessor& processor_ref)
    : waveform_(processor_ref, parametric_eq::theme::ANALYSER_WAVEFORM)
    , control_layer_(processor_ref)
{
    addAndMakeVisible(background_);
    addAndMakeVisible(waveform_);
    addAndMakeVisible(control_layer_);
}

/*---------------------------------------------------------------------------
**
*/
void
Graph::resized()
{
    const auto& bounds = getLocalBounds();

    background_.setBounds(bounds);
    waveform_.setBounds(bounds);
    control_layer_.setBounds(bounds);
}

/*---------------------------------------------------------------------------
**
*/
void
Graph::setActiveBand(const parametric_eq::FILTER_ID& active_band) const
{
    control_layer_.setActiveBand(active_band);
}

/*---------------------------------------------------------------------------
** End of File
*/
