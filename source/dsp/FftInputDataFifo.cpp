#include "FftInputDataFifo.h"
#include "ParametricEqFft.h"
#include "ParametricEqGlobals.h"

/*---------------------------------------------------------------------------
**
*/
FftInputDataFifo::FftInputDataFifo()
{
    fifo_.setSize(1, parametric_eq::fft::INPUT_BUFFER_SIZE);
}

/*---------------------------------------------------------------------------
**
*/
void
FftInputDataFifo::pushData(const juce::AudioBuffer< float >& input_buffer)
{
    juce::AudioBuffer< float > mono_buffer(1, input_buffer.getNumSamples());

    // Each sample of our mono buffer is the sum of the samples at that index
    // across all channels of the potentially multichannel input buffer.
    const auto num_channels = input_buffer.getNumChannels();

    for (int sample = 0; sample < input_buffer.getNumSamples(); ++sample) {
        float sample_sum = 0.f;

        for (int channel = 0; channel < num_channels; ++channel) {
            sample_sum += input_buffer.getSample(channel, sample);
        }

        mono_buffer.setSample(0, sample, sample_sum / static_cast< float >(num_channels));
    }

    // Copy the samples from the mono buffer into our FIFO.
    const juce::ScopedLock fifo_lock(fifo_critical_section_);

    const auto size      = juce::jmin(mono_buffer.getNumSamples(), fifo_.getNumSamples());
    auto       read_ptr  = fifo_.getReadPointer(0, size);
    auto       write_ptr = fifo_.getWritePointer(0, 0);

    std::copy_n(read_ptr, (fifo_.getNumSamples() - size), write_ptr);

    read_ptr  = mono_buffer.getReadPointer(0, 0);
    write_ptr = fifo_.getWritePointer(0, fifo_.getNumSamples() - size);

    std::copy_n(read_ptr, size, write_ptr);
}

/*---------------------------------------------------------------------------
**
*/
void
FftInputDataFifo::pullData(juce::AudioBuffer< float >& buffer_to_fill) const
{
    const juce::ScopedLock fifo_lock(fifo_critical_section_);
    buffer_to_fill.makeCopyOf(fifo_);
}

/*---------------------------------------------------------------------------
**
*/
void
FftInputDataFifo::clear()
{
    const juce::ScopedLock fifo_lock(fifo_critical_section_);
    fifo_.clear();
}

/*---------------------------------------------------------------------------
** End of File
*/
