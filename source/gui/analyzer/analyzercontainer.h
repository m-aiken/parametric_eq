#pragma once

#include <JuceHeader.h>

#include "MonoBufferFifo.h"
#include "ResponseCurve.h"
#include "SpectrumAnalyzer.h"
#include "SpectrumGrid.h"

//==============================================================================
struct AnalyzerContainer : juce::Component
{
    AnalyzerContainer(juce::AudioProcessorValueTreeState&       apvts,
                      double                                    sampleRate,
                      MonoBufferFifo<juce::AudioBuffer<float>>& preEqBufferL,
                      MonoBufferFifo<juce::AudioBuffer<float>>& preEqBufferR,
                      MonoBufferFifo<juce::AudioBuffer<float>>& postEqBufferL,
                      MonoBufferFifo<juce::AudioBuffer<float>>& postEqBufferR);

    void resized() override;

    size_t getActiveNodeIndex();

private:
    std::unique_ptr<SpectrumGrid>     backgroundGrid;
    std::unique_ptr<SpectrumAnalyzer> spectrumAnalyzerPreEq;
    std::unique_ptr<SpectrumAnalyzer> spectrumAnalyzerPostEq;
    std::unique_ptr<ResponseCurve>    responseCurve;
};
