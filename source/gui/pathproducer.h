#pragma once

#include <JuceHeader.h>

#include "AnalyzerPathGenerator.h"
#include "FftDataGenerator.h"
#include "MonoBufferFifo.h"

class PathProducer final : public juce::Thread
{
public:
    PathProducer(double _sampleRate, MonoBufferFifo& scsf);
    ~PathProducer() override;

    void   run() override;
    double getBinWidth() const;
    void   pauseThread();
    void   setFFTRectBounds(const juce::Rectangle< float >& bounds);

    bool pull(juce::Path& path);
    int  getNumAvailableForReading() const;
    void toggleProcessing(bool toggleState);

private:
    MonoBufferFifo& singleChannelSampleFifo;

    FftDataGenerator      fftDataGenerator;
    AnalyzerPathGenerator pathGenerator;

    std::vector< float > renderData;

    void updateRenderData(const std::vector< float >& fftData, float decayRate);

    juce::AudioBuffer< float > bufferForGenerator;

    double                   sampleRate;
    juce::Rectangle< float > fftBounds;

    std::atomic< bool > processingIsEnabled { false };

    static constexpr float DECAY_RATE_DB_PER_SECOND = 30.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathProducer)
};
