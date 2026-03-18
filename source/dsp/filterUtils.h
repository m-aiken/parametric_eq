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

    static void updatePeakCoefficients(MonoChain&                 monoChain,
                                       const ChainPositions&      chainPosition,
                                       juce::AudioParameterFloat* freqParam,
                                       juce::AudioParameterFloat* qParam,
                                       juce::AudioParameterFloat* gainParam,
                                       double                     sampleRate);

    static void updateCutCoefficients(MonoChain&                  monoChain,
                                      const ChainPositions&       chainPosition,
                                      CoefficientsType&           coefficients,
                                      juce::AudioParameterChoice* slopeParam);

    static CoefficientsType
    makeHighPassFilter(juce::AudioParameterFloat* freqParam, juce::AudioParameterChoice* slopeParam, double sampleRate);

    static CoefficientsType
    makeLowPassFilter(juce::AudioParameterFloat* freqParam, juce::AudioParameterChoice* slopeParam, double sampleRate);

    static void updateBandEnablements(MonoChain&                                         monoChain,
                                      juce::AudioParameterBool*                          lcEnabledParam,
                                      juce::AudioParameterBool*                          hcEnabledParam,
                                      std::array< PeakBand, GraphicEq::NUM_PEAK_BANDS >& peakBands);
};
