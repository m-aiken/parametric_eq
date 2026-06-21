#pragma once

#include <JuceHeader.h>

#include "ParametricEqGlobals.h"

class ParametricEqProcessor;

class FilterFactory
{
public:
    explicit FilterFactory(ParametricEqProcessor& processor_ref);

    void updateCutCoefficients(parametric_eq::FilterChain& filters, const parametric_eq::FILTER_ID& filter_id) const;
    void updateLowShelfCoefficients(parametric_eq::FilterChain& filters) const;
    void updateHighShelfCoefficients(parametric_eq::FilterChain& filters) const;
    void updatePeakCoefficients(parametric_eq::FilterChain& filters, const parametric_eq::FILTER_ID& filter_id) const;
    void updateBandEnablements(parametric_eq::FilterChain& filters) const;

private:
    ParametricEqProcessor& processor_ref_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterFactory)
};
