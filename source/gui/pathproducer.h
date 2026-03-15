#pragma once

#include <JuceHeader.h>

#include "analyzerpathgenerator.h"
#include "fftdatagenerator.h"
#include "monobufferfifo.h"
#include "globals.h"

//==============================================================================
struct PathProducer : juce::Thread
{
    PathProducer(double _sampleRate, MonoBufferFifo<juce::AudioBuffer<float>>& scsf);
    ~PathProducer() override;

    void   run() override;
    void   changeOrder(Globals::FFTOrder order);
    int    getFFTSize() const;
    double getBinWidth() const;
    void   pauseThread();
    void   setFFTRectBounds(juce::Rectangle<float> bounds);

    void setDecayRate(float decayRate);
    bool pull(juce::Path& path);
    int  getNumAvailableForReading() const;
    void toggleProcessing(bool toggleState);
    void changePathRange(float negativeInfinityDb, float maxDb);

private:
    MonoBufferFifo<juce::AudioBuffer<float>>* singleChannelSampleFifo;

    std::unique_ptr<FFTDataGenerator>      fftDataGenerator;
    std::unique_ptr<AnalyzerPathGenerator> pathGenerator;

    std::vector<float> renderData;

    void updateRenderData(std::vector<float>&       renData,
                          const std::vector<float>& fftData,
                          int                       numBins,
                          float                     decayRate);

    juce::AudioBuffer<float> bufferForGenerator;

    double                 sampleRate;
    juce::Rectangle<float> fftBounds;

    std::atomic<float> decayRateInDbPerSec { 0.f };
    std::atomic<float> negativeInfinity { Globals::getNegativeInf() };
    std::atomic<float> maxDecibels { Globals::getMaxDecibels() };
    std::atomic<bool>  processingIsEnabled { false };
};
