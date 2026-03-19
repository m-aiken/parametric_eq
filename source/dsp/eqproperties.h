#pragma once

#include <JuceHeader.h>

#include "GraphicEqGlobals.h"

namespace EqProperties
{

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
inline void
addCutParams(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    juce::StringArray cutChoices;

    for (auto i = 0; i < 4; ++i) {
        juce::String str;
        str << (12 + i * 12);
        str << " dB/Oct";
        cutChoices.add(str);
    }

    layout.add(std::make_unique< juce::AudioParameterFloat >(
        juce::ParameterID(GraphicEq::LOW_CUT_FREQ.id_, 1),
        GraphicEq::LOW_CUT_FREQ.id_,
        juce::NormalisableRange(GraphicEq::MIN_HZ, GraphicEq::MAX_HZ, 1.f, 0.2f),
        50.f));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(GraphicEq::LOW_CUT_SLOPE.id_, 1),
                                                              GraphicEq::LOW_CUT_SLOPE.id_,
                                                              cutChoices,
                                                              3));

    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(GraphicEq::LOW_CUT_ENABLED.id_, 1),
                                                            GraphicEq::LOW_CUT_ENABLED.id_,
                                                            false));

    layout.add(std::make_unique< juce::AudioParameterFloat >(
        juce::ParameterID(GraphicEq::HIGH_CUT_FREQ.id_, 1),
        GraphicEq::HIGH_CUT_FREQ.id_,
        juce::NormalisableRange(GraphicEq::MIN_HZ, GraphicEq::MAX_HZ, 1.f, 1.f),
        15000.f));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(GraphicEq::HIGH_CUT_SLOPE.id_, 1),
                                                              GraphicEq::HIGH_CUT_SLOPE.id_,
                                                              cutChoices,
                                                              3));

    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(GraphicEq::HIGH_CUT_ENABLED.id_, 1),
                                                            GraphicEq::HIGH_CUT_ENABLED.id_,
                                                            false));
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
        const auto& freq_name = getPeakControlParamName(PeakControl::FREQUENCY, i);
        const auto& gain_name = getPeakControlParamName(PeakControl::GAIN, i);
        const auto& qual_name = getPeakControlParamName(PeakControl::QUALITY, i);
        const auto& enab_name = getPeakControlParamName(PeakControl::ENABLED, i);

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(freq_name, 1),
                                                                 freq_name,
                                                                 freqNormalisableRange,
                                                                 defaultFreqs.at(i)));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(gain_name, 1),
                                                                 gain_name,
                                                                 gainNormalisableRange,
                                                                 0.f));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(qual_name, 1),
                                                                 qual_name,
                                                                 juce::NormalisableRange(0.1f, 10.f, 0.05f, 1.f),
                                                                 1.f));

        layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(enab_name, 1), enab_name, true));
    }
}

}  // namespace EqProperties
