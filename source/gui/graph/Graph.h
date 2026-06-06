#pragma once

#include <JuceHeader.h>

#include "GraphBackground.h"
#include "GraphControlLayer.h"
#include "GraphWaveform.h"

class Graph final : public juce::Component
{
public:
    explicit Graph(ParametricEqProcessor& processor_ref);

    void resized() override;

    void setActiveBand(const ParametricEq::FILTER_ID& active_band) const;

private:
    GraphBackground   background_;
    GraphWaveform     waveform_pre_eq_;
    GraphWaveform     waveform_post_eq_;
    GraphControlLayer control_layer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Graph)
};
