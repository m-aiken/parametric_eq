#pragma once

#include <JuceHeader.h>

#include "ParametricEqGlobals.h"

class ParametricEqProcessor;

class FilterFactory
{
public:
    explicit FilterFactory(ParametricEqProcessor& processor_ref);

    void updateCutCoefficients(ParametricEq::FilterChain& filter_chain, const ParametricEq::FILTER_ID& filter_id) const;
    void updateLowShelfCoefficients(ParametricEq::FilterChain& filter_chain) const;
    void updateHighShelfCoefficients(ParametricEq::FilterChain& filter_chain) const;
    void updatePeakCoefficients(ParametricEq::FilterChain& filter_chain, const ParametricEq::FILTER_ID& filter_id) const;
    void updateBandEnablements(ParametricEq::FilterChain& filter_chain) const;

private:
    ParametricEqProcessor& processor_ref_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterFactory)
};
