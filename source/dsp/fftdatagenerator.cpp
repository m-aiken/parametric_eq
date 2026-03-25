#include "FftDataGenerator.h"
#include "GraphicEqGlobals.h"

/*---------------------------------------------------------------------------
**
*/
FftDataGenerator::FftDataGenerator()
    : forward_fft_(FFT_ORDER)
    , windowing_func_(FFT_SIZE, juce::dsp::WindowingFunction< float >::blackmanHarris)
{
    fft_data_.clear();
    fft_data_.resize(FFT_SIZE * 2, 0.f);

    fft_data_fifo_.prepare(fft_data_.size());
}

/*---------------------------------------------------------------------------
**
*/
void
FftDataGenerator::produceFFTDataForRendering(const juce::AudioBuffer< float >& audio_data)
{
    std::fill(fft_data_.begin(), fft_data_.end(), 0.f);

    auto* bufferReadIdx = audio_data.getReadPointer(0);
    std::copy(bufferReadIdx, bufferReadIdx + FFT_SIZE, fft_data_.begin());

    windowing_func_.multiplyWithWindowingTable(fft_data_.data(), static_cast< size_t >(FFT_SIZE));

    forward_fft_.performFrequencyOnlyForwardTransform(fft_data_.data(), true);

    juce::FloatVectorOperations::multiply(fft_data_.data(), 1.f / static_cast< float >(NUM_BINS), NUM_BINS + 1);

    for (auto i = 0; i < NUM_BINS + 1; ++i) {
        fft_data_[i] = juce::Decibels::gainToDecibels(fft_data_[i], GraphicEq::MIN_DB);
    }

    fft_data_fifo_.push(fft_data_);
}

/*---------------------------------------------------------------------------
**
*/
int
FftDataGenerator::getNumAvailableFFTDataBlocks() const
{
    return fft_data_fifo_.getNumAvailableForReading();
}

/*---------------------------------------------------------------------------
**
*/
bool
FftDataGenerator::getFFTData(std::vector< float >& data)
{
    return fft_data_fifo_.pull(data);
}

/*---------------------------------------------------------------------------
** End of File
*/
