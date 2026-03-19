#include "PathProducer.h"

/*---------------------------------------------------------------------------
**
*/
PathProducer::PathProducer(double _sampleRate, MonoBufferFifo< juce::AudioBuffer< float > >& scsf)
    : juce::Thread("PathProducerThread")
    , singleChannelSampleFifo(&scsf)
    , sampleRate(_sampleRate)
{
    fftDataGenerator = std::make_unique< FftDataGenerator >();
    pathGenerator    = std::make_unique< AnalyzerPathGenerator >();

    // TODO - initialising these 2 might not be necessary ultimately.
    // It is necessary at the time of writing this as the associated parameters don't exist in the value tree yet.
    bufferForGenerator.setSize(1, getFFTSize());
    renderData.resize(getFFTSize(), GraphicEq::MIN_DB);

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
    if (singleChannelSampleFifo == nullptr || fftDataGenerator == nullptr || pathGenerator == nullptr) {
        return;
    }

    while (!threadShouldExit()) {
        if (!processingIsEnabled.load()) {
            wait(10);
            continue;
        }

        while (singleChannelSampleFifo->getNumCompleteBuffersAvailable() > 0) {
            juce::AudioBuffer< float > tempBuffer;

            if (threadShouldExit())
                break;

            if (singleChannelSampleFifo->getAudioBuffer(tempBuffer)) {
                auto size     = juce::jmin(tempBuffer.getNumSamples(), bufferForGenerator.getNumSamples());
                auto readPtr  = bufferForGenerator.getReadPointer(0, size);
                auto writePtr = bufferForGenerator.getWritePointer(0, 0);
                std::copy(readPtr, readPtr + (bufferForGenerator.getNumSamples() - size), writePtr);

                juce::FloatVectorOperations::copy(
                    bufferForGenerator.getWritePointer(0, bufferForGenerator.getNumSamples() - size),
                    tempBuffer.getReadPointer(0, 0),
                    size);

                fftDataGenerator->produceFFTDataForRendering(bufferForGenerator);
            }
        }

        while (fftDataGenerator->getNumAvailableFFTDataBlocks() > 0) {
            std::vector< float > fftData;

            if (threadShouldExit())
                break;

            if (fftDataGenerator->getFFTData(fftData)) {
                auto fftSize   = getFFTSize();
                auto numBins   = static_cast< int >(fftSize * 0.5);
                auto decayRate = DECAY_RATE_DB_PER_SECOND / 60.f;

                updateRenderData(renderData, fftData, numBins, decayRate);

                pathGenerator->generatePath(renderData, fftBounds, fftSize, static_cast< float >(getBinWidth()));
            }
        }

        wait(10);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
PathProducer::changeOrder(GraphicEq::FFTOrder order)
{
    if (fftDataGenerator == nullptr || singleChannelSampleFifo == nullptr) {
        return;
    }

    pauseThread();

    fftDataGenerator->changeOrder(order);

    auto fftSize = getFFTSize();
    renderData.clear();
    renderData.resize(static_cast< size_t >(fftSize * 2), GraphicEq::MIN_DB);
    bufferForGenerator.setSize(1, fftSize);

    while (!singleChannelSampleFifo->isPrepared()) {
        wait(5);
    }

    if (!fftBounds.isEmpty()) {
        startThread();
    }
}

/*---------------------------------------------------------------------------
**
*/
int
PathProducer::getFFTSize() const
{
    // TODO is 0 a good default value for this?
    return (fftDataGenerator != nullptr) ? fftDataGenerator->getFFTSize() : 0;
}

/*---------------------------------------------------------------------------
**
*/
double
PathProducer::getBinWidth() const
{
    return sampleRate / static_cast< double >(getFFTSize());
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
PathProducer::setFFTRectBounds(juce::Rectangle< float > bounds)
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
    return (pathGenerator != nullptr) && pathGenerator->getPath(path);
}

/*---------------------------------------------------------------------------
**
*/
int
PathProducer::getNumAvailableForReading() const
{
    return (pathGenerator != nullptr) ? pathGenerator->getNumPathsAvailable() : 0;
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
PathProducer::updateRenderData(std::vector< float >&       renData,
                               const std::vector< float >& fftData,
                               int                         numBins,
                               float                       decayRate)
{
    if (decayRate >= 0.f) {
        for (auto i = 0; i < numBins; ++i) {
            auto previousValue = renData[i];
            auto candidate     = fftData[i];
            auto finalValue    = juce::jmax(candidate, previousValue - decayRate);
            renData[i]         = juce::jlimit(GraphicEq::MIN_DB, GraphicEq::MAX_DB, finalValue);
        }
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
