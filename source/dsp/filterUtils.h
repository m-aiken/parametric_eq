#pragma once

#include <JuceHeader.h>

#include "GraphicEqGlobals.h"
#include "PeakBand.h"

struct FilterUtils {
    using Filter = juce::dsp::IIR::Filter< float >;
    // Cut Filter chain: x4 Filters to allow for 12/24/36/48 dB/oct options
    using CutFilter = juce::dsp::ProcessorChain< Filter, Filter, Filter, Filter >;
    using MonoChain = juce::dsp::ProcessorChain< CutFilter, Filter, Filter, Filter, Filter, Filter, Filter, CutFilter >;
    using CoefficientsType = juce::ReferenceCountedArray< juce::dsp::FilterDesign< float >::IIRCoefficients >;

    enum ChainPositions { LowCut, Peak_0, Peak_1, Peak_2, Peak_3, Peak_4, Peak_5, HighCut };

    enum Slope { Slope_12, Slope_24, Slope_36, Slope_48 };

    static void updatePeakCoefficients(MonoChain&            mono_chain,
                                       const ChainPositions& chain_position,
                                       const PeakBand&       peak_band,
                                       const double          sample_rate);

    static void updateCutCoefficients(MonoChain&                        mono_chain,
                                      const ChainPositions&             chain_position,
                                      const CoefficientsType&           coefficients,
                                      const juce::AudioParameterChoice* slope_param);

    static CoefficientsType makeHighPassFilter(const juce::AudioParameterFloat*  freq_param,
                                               const juce::AudioParameterChoice* slope_param,
                                               const double                      sample_rate);

    static CoefficientsType makeLowPassFilter(const juce::AudioParameterFloat*  freq_param,
                                              const juce::AudioParameterChoice* slope_param,
                                              const double                      sample_rate);

    static void updateBandEnablements(MonoChain&                                               mono_chain,
                                      const juce::AudioParameterBool*                          lc_enabled_param,
                                      const juce::AudioParameterBool*                          hc_enabled_param,
                                      const std::array< PeakBand, GraphicEq::NUM_PEAK_BANDS >& peak_bands);
};
