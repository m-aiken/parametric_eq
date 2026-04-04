#pragma once

#include <JuceHeader.h>

#include "GraphicEqGlobals.h"

class GraphicEqProcessor;

class FilterFactory
{
public:
    explicit FilterFactory(GraphicEqProcessor* processor_ref);

    void updateCutCoefficients(GraphicEq::FilterChain& filter_chain, const GraphicEq::FILTER_ID& filter_id) const;
    void updateLowShelfCoefficients(GraphicEq::FilterChain& filter_chain) const;
    void updateHighShelfCoefficients(GraphicEq::FilterChain& filter_chain) const;
    void updatePeakCoefficients(GraphicEq::FilterChain& filter_chain, const GraphicEq::FILTER_ID& filter_id) const;
    void updateBandEnablements(GraphicEq::FilterChain& filter_chain) const;

private:
    GraphicEqProcessor* processor_ref_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterFactory)
};
