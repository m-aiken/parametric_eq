#pragma once

#include <JuceHeader.h>

class GraphicEqProcessor;

class FilterFactory
{
public:
    explicit FilterFactory(GraphicEqProcessor* processor_ref);

    using Filter = juce::dsp::IIR::Filter< float >;
    // Cut Filter chain: x4 Filters to allow for 12/24/36/48 dB/oct options
    using CutFilter = juce::dsp::ProcessorChain< Filter, Filter, Filter, Filter >;
    using FilterChain = juce::dsp::ProcessorChain< CutFilter, Filter, Filter, Filter, Filter, Filter, Filter, CutFilter >;
    using CoefficientsType = juce::ReferenceCountedArray< juce::dsp::FilterDesign< float >::IIRCoefficients >;

    enum ChainPositions {
        LowCut,
        Peak_0,
        Peak_1,
        Peak_2,
        Peak_3,
        Peak_4,
        Peak_5,
        HighCut,
    };

    enum Slope {
        Slope_12,
        Slope_24,
        Slope_36,
        Slope_48,
    };

    void
    updatePeakCoefficients(FilterChain& filter_chain, const ChainPositions& chain_position, const int band_index) const;
    void updateCutCoefficients(FilterChain& filter_chain, const ChainPositions& chain_position) const;
    void updateBandEnablements(FilterChain& filter_chain) const;

private:
    GraphicEqProcessor* processor_ref_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterFactory)
};
