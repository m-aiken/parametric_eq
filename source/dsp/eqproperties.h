#pragma once

#include <JuceHeader.h>

#include "GraphicEqGlobals.h"

namespace EqProperties
{

enum CutControls {
    LOW_CUT_FREQ,
    LOW_CUT_SLOPE,
    LOW_CUT_ENABLED,
    HIGH_CUT_FREQ,
    HIGH_CUT_SLOPE,
    HIGH_CUT_ENABLED,
};

enum class PeakControl {
    FREQUENCY,
    GAIN,
    QUALITY,
    ENABLED,
};

/*---------------------------------------------------------------------------
**
*/
static std::map< PeakControl, juce::String > peakControlMap = { { PeakControl::FREQUENCY, "Freq" },
                                                                { PeakControl::GAIN, "Gain" },
                                                                { PeakControl::QUALITY, "Q" },
                                                                { PeakControl::ENABLED, "Enabled" } };

/*---------------------------------------------------------------------------
**
*/
inline const juce::String
getPeakControlParamName(PeakControl peakControl, const int bandNum)
{
    juce::String str;
    str << "P" << bandNum << " " << peakControlMap.at(peakControl);
    return str;
}

/*---------------------------------------------------------------------------
**
*/
inline const std::map< CutControls, juce::String >&
getCutParams()
{
    static std::map< CutControls, juce::String > paramNamesMap = {
        { LOW_CUT_FREQ, "Low Cut Freq" },       { LOW_CUT_SLOPE, "Low Cut Slope" },
        { LOW_CUT_ENABLED, "Low Cut Enabled" }, { HIGH_CUT_FREQ, "High Cut Freq" },
        { HIGH_CUT_SLOPE, "High Cut Slope" },   { HIGH_CUT_ENABLED, "High Cut Enabled" }
    };

    return paramNamesMap;
}

/*---------------------------------------------------------------------------
**
*/
inline void
addCutParams(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const auto& params = getCutParams();

    juce::StringArray cutChoices;

    for (auto i = 0; i < 4; ++i) {
        juce::String str;
        str << (12 + i * 12);
        str << " dB/Oct";
        cutChoices.add(str);
    }

    layout.add(std::make_unique< juce::AudioParameterFloat >(
        params.at(LOW_CUT_FREQ),
        params.at(LOW_CUT_FREQ),
        juce::NormalisableRange(GraphicEq::MIN_HZ, GraphicEq::MAX_HZ, 1.f, 0.2f),
        50.f));

    layout.add(
        std::make_unique< juce::AudioParameterChoice >(params.at(LOW_CUT_SLOPE), params.at(LOW_CUT_SLOPE), cutChoices, 3));

    layout.add(
        std::make_unique< juce::AudioParameterBool >(params.at(LOW_CUT_ENABLED), params.at(LOW_CUT_ENABLED), false));

    layout.add(std::make_unique< juce::AudioParameterFloat >(
        params.at(HIGH_CUT_FREQ),
        params.at(HIGH_CUT_FREQ),
        juce::NormalisableRange(GraphicEq::MIN_HZ, GraphicEq::MAX_HZ, 1.f, 1.f),
        15000.f));

    layout.add(std::make_unique< juce::AudioParameterChoice >(params.at(HIGH_CUT_SLOPE),
                                                              params.at(HIGH_CUT_SLOPE),
                                                              cutChoices,
                                                              3));

    layout.add(
        std::make_unique< juce::AudioParameterBool >(params.at(HIGH_CUT_ENABLED), params.at(HIGH_CUT_ENABLED), false));
}

/*---------------------------------------------------------------------------
**
*/
inline void
addPeakParams(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    auto freqNormalisableRange = juce::NormalisableRange(GraphicEq::MIN_HZ, GraphicEq::MAX_HZ, 1.f, 1.f);
    auto gainNormalisableRange = juce::NormalisableRange(GraphicEq::MIN_DB, GraphicEq::MAX_DB, 0.5f, 1.f);

    std::vector< float > defaultFreqs { 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f };

    for (int i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        layout.add(std::make_unique< juce::AudioParameterFloat >(getPeakControlParamName(PeakControl::FREQUENCY, i),
                                                                 getPeakControlParamName(PeakControl::FREQUENCY, i),
                                                                 freqNormalisableRange,
                                                                 defaultFreqs.at(i)));

        layout.add(std::make_unique< juce::AudioParameterFloat >(getPeakControlParamName(PeakControl::GAIN, i),
                                                                 getPeakControlParamName(PeakControl::GAIN, i),
                                                                 gainNormalisableRange,
                                                                 0.f));

        layout.add(std::make_unique< juce::AudioParameterFloat >(getPeakControlParamName(PeakControl::QUALITY, i),
                                                                 getPeakControlParamName(PeakControl::QUALITY, i),
                                                                 juce::NormalisableRange(0.1f, 10.f, 0.05f, 1.f),
                                                                 1.f));

        layout.add(std::make_unique< juce::AudioParameterBool >(getPeakControlParamName(PeakControl::ENABLED, i),
                                                                getPeakControlParamName(PeakControl::ENABLED, i),
                                                                true));
    }
}

}  // namespace EqProperties
