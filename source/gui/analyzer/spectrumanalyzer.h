#pragma once

#include <JuceHeader.h>

#include "MonoBufferFifo.h"
#include "ParamListener.h"
#include "PathProducer.h"

class SpectrumAnalyzer final
    : public juce::Component
    , public juce::Timer
{
public:
    SpectrumAnalyzer(double                                        _sampleRate,
                     MonoBufferFifo< juce::AudioBuffer< float > >& leftScsf,
                     MonoBufferFifo< juce::AudioBuffer< float > >& rightScsf,
                     juce::AudioProcessorValueTreeState&           apvts,
                     const juce::Colour&                           fillColour);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    double       sampleRate;
    juce::Colour colour;
    juce::Path   leftAnalyzerPath;
    juce::Path   rightAnalyzerPath;
    PathProducer leftPathProducer;
    PathProducer rightPathProducer;
    //    bool         active { false };
    bool active { true };  // TODO for test only

    void setActive(bool activeState);
    void updateOrder(float value);
    void animate();

    ParamListener< float > analyzerOrderParamListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzer)
};
