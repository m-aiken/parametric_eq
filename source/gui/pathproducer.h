#pragma once

#include <JuceHeader.h>

#include "AnalyzerPathGenerator.h"
#include "FftDataGenerator.h"
#include "MonoBufferFifo.h"
#include "GraphicEqGlobals.h"

class PathProducer final : public juce::Thread
{
public:
    PathProducer(double _sampleRate, MonoBufferFifo< juce::AudioBuffer< float > >& scsf);
    ~PathProducer() override;

    void   run() override;
    void   changeOrder(GraphicEq::FFTOrder order);
    int    getFFTSize() const;
    double getBinWidth() const;
    void   pauseThread();
    void   setFFTRectBounds(juce::Rectangle< float > bounds);

    bool pull(juce::Path& path);
    int  getNumAvailableForReading() const;
    void toggleProcessing(bool toggleState);

private:
    MonoBufferFifo< juce::AudioBuffer< float > >* singleChannelSampleFifo;

    std::unique_ptr< FftDataGenerator >      fftDataGenerator;
    std::unique_ptr< AnalyzerPathGenerator > pathGenerator;

    std::vector< float > renderData;

    void
    updateRenderData(std::vector< float >& renData, const std::vector< float >& fftData, int numBins, float decayRate);

    juce::AudioBuffer< float > bufferForGenerator;

    double                   sampleRate;
    juce::Rectangle< float > fftBounds;

    std::atomic< bool > processingIsEnabled { false };

    static constexpr float DECAY_RATE_DB_PER_SECOND = 30.f;
};
