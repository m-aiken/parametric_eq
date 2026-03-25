#include "PathProducer.h"

/*---------------------------------------------------------------------------
**
*/
PathProducer::PathProducer(double _sampleRate, MonoBufferFifo& scsf)
    : juce::Thread("PathProducerThread")
    , singleChannelSampleFifo(scsf)
    , sampleRate(_sampleRate)
{
    renderData.clear();
    renderData.resize(static_cast< size_t >(FftDataGenerator::FFT_SIZE * 2), GraphicEq::MIN_DB);
    bufferForGenerator.setSize(1, FftDataGenerator::FFT_SIZE);

    startThread();
}

/*---------------------------------------------------------------------------
**
*/
PathProducer::~PathProducer()
{
    notify();
    stopThread(100);
}

/*---------------------------------------------------------------------------
**
*/
void
PathProducer::run()
{
    while (!threadShouldExit()) {
        if (!processingIsEnabled.load()) {
            wait(10);
            continue;
        }

        while (singleChannelSampleFifo.getNumCompleteBuffersAvailable() > 0) {
            juce::AudioBuffer< float > tempBuffer;

            if (threadShouldExit())
                break;

            if (singleChannelSampleFifo.getAudioBuffer(tempBuffer)) {
                auto size     = juce::jmin(tempBuffer.getNumSamples(), bufferForGenerator.getNumSamples());
                auto readPtr  = bufferForGenerator.getReadPointer(0, size);
                auto writePtr = bufferForGenerator.getWritePointer(0, 0);
                std::copy(readPtr, readPtr + (bufferForGenerator.getNumSamples() - size), writePtr);

                juce::FloatVectorOperations::copy(
                    bufferForGenerator.getWritePointer(0, bufferForGenerator.getNumSamples() - size),
                    tempBuffer.getReadPointer(0, 0),
                    size);

                fftDataGenerator.produceFFTDataForRendering(bufferForGenerator);
            }
        }

        while (fftDataGenerator.getNumAvailableFFTDataBlocks() > 0) {
            std::vector< float > fftData;

            if (threadShouldExit())
                break;

            if (fftDataGenerator.getFFTData(fftData)) {
                auto decayRate = DECAY_RATE_DB_PER_SECOND / 60.f;

                updateRenderData(fftData, decayRate);

                pathGenerator.generatePath(renderData,
                                           fftBounds,
                                           FftDataGenerator::FFT_SIZE,
                                           static_cast< float >(getBinWidth()));
            }
        }

        wait(10);
    }
}

/*---------------------------------------------------------------------------
**
*/
double
PathProducer::getBinWidth() const
{
    return sampleRate / static_cast< double >(FftDataGenerator::FFT_SIZE);
}

/*---------------------------------------------------------------------------
**
*/
void
PathProducer::pauseThread()
{
    stopThread(100);
}

/*---------------------------------------------------------------------------
**
*/
void
PathProducer::setFFTRectBounds(const juce::Rectangle< float >& bounds)
{
    pauseThread();

    if (!bounds.isEmpty()) {
        fftBounds = bounds;
    }

    startThread();
}

/*---------------------------------------------------------------------------
**
*/
bool
PathProducer::pull(juce::Path& path)
{
    return pathGenerator.getPath(path);
}

/*---------------------------------------------------------------------------
**
*/
int
PathProducer::getNumAvailableForReading() const
{
    return pathGenerator.getNumPathsAvailable();
}

/*---------------------------------------------------------------------------
**
*/
void
PathProducer::toggleProcessing(bool toggleState)
{
    processingIsEnabled.store(toggleState);
}

/*---------------------------------------------------------------------------
**
*/
void
PathProducer::updateRenderData(const std::vector< float >& fftData, float decayRate)
{
    if (decayRate >= 0.f) {
        for (auto i = 0; i < FftDataGenerator::NUM_BINS; ++i) {
            auto previousValue = renderData[i];
            auto candidate     = fftData[i];
            auto finalValue    = juce::jmax(candidate, previousValue - decayRate);
            renderData[i]      = juce::jlimit(GraphicEq::MIN_DB, GraphicEq::MAX_DB, finalValue);
        }
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
