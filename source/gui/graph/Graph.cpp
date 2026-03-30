#include "Graph.h"
#include "GraphicEqProcessor.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
Graph::Graph(GraphicEqProcessor& processor_ref)
    : waveform_pre_eq_(processor_ref.getSampleRate(),
                       processor_ref.preEqMonoFifoL,
                       processor_ref.preEqMonoFifoR,
                       Theme::ANALYSER_PRE_EQ)
    , waveform_post_eq_(processor_ref.getSampleRate(),
                        processor_ref.postEqMonoFifoL,
                        processor_ref.postEqMonoFifoR,
                        Theme::ANALYSER_POST_EQ)
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
size_t
Graph::getActiveNodeIndex() const
{
    return control_layer_.getActiveNodeIndex();
}

/*---------------------------------------------------------------------------
** End of File
*/
