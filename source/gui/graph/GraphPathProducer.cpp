#include "GraphPathProducer.h"

/*---------------------------------------------------------------------------
**
*/
GraphPathProducer::GraphPathProducer(const double sample_rate, MonoBufferFifo& scsf)
    : juce::Thread("PathProducerThread")
    , single_channel_sample_fifo_(scsf)
    , sample_rate_(sample_rate)
{
    render_data_.clear();
    render_data_.resize(static_cast< size_t >(FftDataGenerator::FFT_SIZE * 2), GraphicEq::MIN_DB);
    buffer_for_generator_.setSize(1, FftDataGenerator::FFT_SIZE);

    startThread();
}

/*---------------------------------------------------------------------------
**
*/
GraphPathProducer::~GraphPathProducer()
{
    notify();
    stopThread(100);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::run()
{
    while (!threadShouldExit()) {
        if (!processing_is_enabled_.load()) {
            (void) wait(10);
            continue;
        }

        while (single_channel_sample_fifo_.getNumCompleteBuffersAvailable() > 0) {
            if (threadShouldExit()) {
                break;
            }

            if (juce::AudioBuffer< float > temp_buffer; single_channel_sample_fifo_.getAudioBuffer(temp_buffer)) {
                const auto size      = juce::jmin(temp_buffer.getNumSamples(), buffer_for_generator_.getNumSamples());
                const auto read_ptr  = buffer_for_generator_.getReadPointer(0, size);
                const auto write_ptr = buffer_for_generator_.getWritePointer(0, 0);

                std::copy_n(read_ptr, (buffer_for_generator_.getNumSamples() - size), write_ptr);

                juce::FloatVectorOperations::copy(
                    buffer_for_generator_.getWritePointer(0, buffer_for_generator_.getNumSamples() - size),
                    temp_buffer.getReadPointer(0, 0),
                    size);

                fft_data_generator_.produceFFTDataForRendering(buffer_for_generator_);
            }
        }

        while (fft_data_generator_.getNumAvailableFFTDataBlocks() > 0) {
            if (threadShouldExit()) {
                break;
            }

            if (std::vector< float > fft_data; fft_data_generator_.getFFTData(fft_data)) {
                updateRenderData(fft_data);

                path_generator_.generatePath(render_data_,
                                             fft_bounds_,
                                             FftDataGenerator::FFT_SIZE,
                                             static_cast< float >(getBinWidth()));
            }
        }

        (void) wait(10);
    }
}

/*---------------------------------------------------------------------------
**
*/
double
GraphPathProducer::getBinWidth() const
{
    return sample_rate_ / static_cast< double >(FftDataGenerator::FFT_SIZE);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::pauseThread()
{
    stopThread(100);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::setFFTRectBounds(const juce::Rectangle< float >& bounds)
{
    pauseThread();

    if (!bounds.isEmpty()) {
        fft_bounds_ = bounds;
    }

    startThread();
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphPathProducer::pull(juce::Path& path)
{
    return path_generator_.getPath(path);
}

/*---------------------------------------------------------------------------
**
*/
int
GraphPathProducer::getNumAvailableForReading() const
{
    return path_generator_.getNumPathsAvailable();
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::toggleProcessing(const bool toggle_state)
{
    processing_is_enabled_.store(toggle_state);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphPathProducer::updateRenderData(const std::vector< float >& fft_data)
{
    for (auto i = 0; i < FftDataGenerator::NUM_BINS; ++i) {
        const auto previous_value = render_data_[i];
        const auto candidate      = fft_data[i];
        const auto final_value    = juce::jmax(candidate, previous_value - DECAY_RATE);
        render_data_[i]           = juce::jlimit(GraphicEq::MIN_DB, GraphicEq::MAX_DB, final_value);
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
