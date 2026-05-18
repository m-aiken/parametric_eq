#pragma once

#include <JuceHeader.h>

#include "GraphBackground.h"
#include "GraphControlLayer.h"
#ifdef SHOW_WAVEFORM
    #include "GraphWaveform.h"
#endif

class Graph final : public juce::Component
{
public:
    explicit Graph(ParametricEqProcessor& processor_ref);

    void resized() override;

    void setActiveBand(const ParametricEq::FILTER_ID& active_band) const;

private:
    GraphBackground background_;
#ifdef SHOW_WAVEFORM
    GraphWaveform waveform_pre_eq_;
    GraphWaveform waveform_post_eq_;
#endif
    GraphControlLayer control_layer_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Graph)
};
