#include "Graph.h"
#include "ParametricEqProcessor.h"
#ifdef SHOW_WAVEFORM
    #include "Theme.h"
#endif

/*---------------------------------------------------------------------------
**
*/
Graph::Graph(ParametricEqProcessor& processor_ref)
#ifdef SHOW_WAVEFORM
    : waveform_pre_eq_(processor_ref.getSampleRate(), processor_ref.getPreEqFftFifo(), Theme::ANALYSER_PRE_EQ)
    , waveform_post_eq_(processor_ref.getSampleRate(), processor_ref.getPostEqFftFifo(), Theme::ANALYSER_POST_EQ)
    , control_layer_(processor_ref)
#else
    : control_layer_(processor_ref)
#endif
{
    addAndMakeVisible(background_);
#ifdef SHOW_WAVEFORM
    addAndMakeVisible(waveform_pre_eq_);
    addAndMakeVisible(waveform_post_eq_);
#endif
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
#ifdef SHOW_WAVEFORM
    waveform_pre_eq_.setBounds(bounds);
    waveform_post_eq_.setBounds(bounds);
#endif
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
