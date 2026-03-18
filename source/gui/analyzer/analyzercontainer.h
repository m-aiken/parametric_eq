#pragma once

#include <JuceHeader.h>

#include "MonoBufferFifo.h"
#include "ResponseCurve.h"
#include "SpectrumAnalyzer.h"
#include "SpectrumGrid.h"

class AnalyzerContainer final : public juce::Component
{
public:
    AnalyzerContainer(juce::AudioProcessorValueTreeState&           apvts,
                      double                                        sampleRate,
                      MonoBufferFifo< juce::AudioBuffer< float > >& preEqBufferL,
                      MonoBufferFifo< juce::AudioBuffer< float > >& preEqBufferR,
                      MonoBufferFifo< juce::AudioBuffer< float > >& postEqBufferL,
                      MonoBufferFifo< juce::AudioBuffer< float > >& postEqBufferR);

    void resized() override;

    size_t getActiveNodeIndex();

private:
    SpectrumGrid     backgroundGrid;
    SpectrumAnalyzer spectrumAnalyzerPreEq;
    SpectrumAnalyzer spectrumAnalyzerPostEq;
    ResponseCurve    responseCurve;
};
