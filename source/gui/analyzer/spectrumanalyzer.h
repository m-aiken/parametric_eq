#pragma once

#include <JuceHeader.h>

#include "MonoBufferFifo.h"
#include "ParamListener.h"
#include "PathProducer.h"

//==============================================================================
struct SpectrumAnalyzer : juce::Component, juce::Timer
{
    SpectrumAnalyzer(double                                    _sampleRate,
                     MonoBufferFifo<juce::AudioBuffer<float>>& leftScsf,
                     MonoBufferFifo<juce::AudioBuffer<float>>& rightScsf,
                     juce::AudioProcessorValueTreeState&       apvts,
                     const juce::Colour&                       fillColour);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    double                        sampleRate;
    juce::Colour                  colour;
    std::unique_ptr<juce::Path>   leftAnalyzerPath;
    std::unique_ptr<juce::Path>   rightAnalyzerPath;
    std::unique_ptr<PathProducer> leftPathProducer;
    std::unique_ptr<PathProducer> rightPathProducer;
    //    bool         active { false };
    bool active { true }; // TODO for test only

    void setActive(bool activeState);
    void updateDecayRate(float decayRate);
    void updateOrder(float value);
    void animate();

    std::unique_ptr<ParamListener<float>> analyzerOrderParamListener;
    std::unique_ptr<ParamListener<float>> analyzerDecayRateParamListener;
};
