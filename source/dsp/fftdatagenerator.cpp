#include "FftDataGenerator.h"

/*---------------------------------------------------------------------------
**
*/
void
FftDataGenerator::produceFFTDataForRendering(const juce::AudioBuffer< float >& audioData)
{
    const auto fftSize = getFFTSize();

    std::fill(fftData.begin(), fftData.end(), 0.f);

    auto* bufferReadIdx = audioData.getReadPointer(0);
    std::copy(bufferReadIdx, bufferReadIdx + fftSize, fftData.begin());

    if (window != nullptr) {
        window->multiplyWithWindowingTable(fftData.data(), static_cast< size_t >(fftSize));
    }

    if (forwardFFT != nullptr) {
        forwardFFT->performFrequencyOnlyForwardTransform(fftData.data(), true);
    }

    auto numBins = static_cast< int >(fftSize * 0.5);
    juce::FloatVectorOperations::multiply(fftData.data(), 1.f / static_cast< float >(numBins), numBins + 1);

    for (auto i = 0; i < numBins + 1; ++i) {
        fftData[i] = juce::Decibels::gainToDecibels(fftData[i], GraphicEqGlobals::getNegativeInf());
    }

    fftDataFifo.push(fftData);
}

/*---------------------------------------------------------------------------
**
*/
void
FftDataGenerator::changeOrder(GraphicEqGlobals::FFTOrder newOrder)
{
    order      = newOrder;
    forwardFFT = std::make_unique< juce::dsp::FFT >(order);

    auto fftSize = getFFTSize();
    window =
        std::make_unique< juce::dsp::WindowingFunction< float > >(fftSize,
                                                                  juce::dsp::WindowingFunction< float >::blackmanHarris);

    fftData.clear();
    fftData.resize(static_cast< size_t >(fftSize * 2), 0.f);

    fftDataFifo.prepare(fftData.size());
}

/*---------------------------------------------------------------------------
** End of File
*/
