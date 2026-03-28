#include "Graph.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
Graph::Graph(juce::AudioProcessorValueTreeState& apvts,
             const double                        sample_rate,
             MonoBufferFifo&                     pre_eq_buffer_l,
             MonoBufferFifo&                     pre_eq_buffer_r,
             MonoBufferFifo&                     post_eq_buffer_l,
             MonoBufferFifo&                     post_eq_buffer_r)
    : waveform_pre_eq_(sample_rate, pre_eq_buffer_l, pre_eq_buffer_r, Theme::ANALYSER_PRE_EQ)
    , waveform_post_eq_(sample_rate, post_eq_buffer_l, post_eq_buffer_r, Theme::ANALYSER_POST_EQ)
    , control_layer_(apvts, sample_rate)
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
