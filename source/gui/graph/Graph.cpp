#include "Graph.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
Graph::Graph(ParametricEqProcessor& processor_ref)
    : background_(processor_ref)
    , waveform_pre_eq_(processor_ref, processor_ref.getPreEqFftFifo(), Theme::ANALYSER_PRE_EQ)
    , waveform_post_eq_(processor_ref, processor_ref.getPostEqFftFifo(), Theme::ANALYSER_POST_EQ)
    , control_layer_(processor_ref)
{
    addAndMakeVisible(background_);
    addAndMakeVisible(waveform_pre_eq_);
    addAndMakeVisible(waveform_post_eq_);
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
    waveform_pre_eq_.setBounds(bounds);
    waveform_post_eq_.setBounds(bounds);
    control_layer_.setBounds(bounds);
}

/*---------------------------------------------------------------------------
**
*/
void
Graph::setActiveBand(const ParametricEq::FILTER_ID& active_band) const
{
    control_layer_.setActiveBand(active_band);
}

/*---------------------------------------------------------------------------
** End of File
*/
