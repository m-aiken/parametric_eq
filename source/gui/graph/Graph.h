#pragma once

#include <JuceHeader.h>

#include "GraphBackground.h"
#include "GraphControlLayer.h"
#include "GraphWaveform.h"
#include "MonoBufferFifo.h"

class Graph final : public juce::Component
{
public:
    Graph(juce::AudioProcessorValueTreeState& apvts,
          const double                        sample_rate,
          MonoBufferFifo&                     pre_eq_buffer_l,
          MonoBufferFifo&                     pre_eq_buffer_r,
          MonoBufferFifo&                     post_eq_buffer_l,
          MonoBufferFifo&                     post_eq_buffer_r);

    void resized() override;

    size_t getActiveNodeIndex() const;

private:
    GraphBackground   background_;
    GraphWaveform     waveform_pre_eq_;
    GraphWaveform     waveform_post_eq_;
    GraphControlLayer control_layer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Graph)
};
