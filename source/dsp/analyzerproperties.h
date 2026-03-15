#pragma once

#include <JuceHeader.h>

#include "GraphicEqGlobals.h"

//==============================================================================
namespace AnalyzerProperties {

enum ParamNames {
    Analyzer_Points,
    Analyzer_Decay_Rate
};

inline const std::map<ParamNames, juce::String>& getAnalyzerParams()
{
    static std::map<ParamNames, juce::String> paramNamesMap = {
        { Analyzer_Points, "Analyzer Points" },
        { Analyzer_Decay_Rate, "Analyzer Decay Rate" }
    };

    return paramNamesMap;
}

inline const std::map<GraphicEqGlobals::FFTOrder, juce::String>& getAnalyzerPoints()
{
    static std::map<GraphicEqGlobals::FFTOrder, juce::String> fftOrderMap = {
        { GraphicEqGlobals::order2048, "order2048" },
        { GraphicEqGlobals::order4096, "order4096" },
        { GraphicEqGlobals::order8192, "order8192" }
    };

    return fftOrderMap;
}

inline void addAnalyzerParams(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const auto& params    = getAnalyzerParams();
    const auto& fftOrders = getAnalyzerPoints();

    layout.add(std::make_unique<juce::AudioParameterChoice>(params.at(Analyzer_Points),
                                                            params.at(Analyzer_Points),
                                                            juce::StringArray { fftOrders.at(GraphicEqGlobals::order2048), fftOrders.at(GraphicEqGlobals::order4096), fftOrders.at(GraphicEqGlobals::order8192) },
                                                            0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(params.at(Analyzer_Decay_Rate),
                                                           params.at(Analyzer_Decay_Rate),
                                                           juce::NormalisableRange<float>(0.f, 30.f, 0.1f, 1.f),
                                                           30.f));
}

}
