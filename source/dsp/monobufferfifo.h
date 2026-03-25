#pragma once

#include <JuceHeader.h>

#include "Fifo.h"
#include "GraphicEqGlobals.h"

class MonoBufferFifo
{
public:
    explicit MonoBufferFifo(const GraphicEq::Channel ch)
        : channel_(ch)
        , fifo_index_(0)
        , prepared_(false)
        , size_(0)
    {
    }

    void update(const juce::AudioBuffer< float >& buffer)
    {
        for (auto i = 0; i < buffer.getNumSamples(); ++i) {
            pushNextSampleIntoFifo(buffer.getSample(channel_, i));
        }
    }

    void pushNextSampleIntoFifo(float sample)
    {
        if (fifo_index_ == getSize()) {
            audio_buffer_fifo_.push(buffer_to_fill_);
            fifo_index_ = 0;
        }

        buffer_to_fill_.setSample(0, fifo_index_, sample);
        ++fifo_index_;
    }

    void prepare(int buffer_size)
    {
        prepared_.set(false);
        size_.set(buffer_size);
        audio_buffer_fifo_.prepare(buffer_size, 1);
        buffer_to_fill_.setSize(1, buffer_size);
        fifo_index_ = 0;
        prepared_.set(true);
    }

    int  getNumCompleteBuffersAvailable() const { return audio_buffer_fifo_.getNumAvailableForReading(); }
    bool getAudioBuffer(juce::AudioBuffer< float >& buffer) { return audio_buffer_fifo_.pull(buffer); }
    bool isPrepared() const { return prepared_.get(); }
    int  getSize() const { return size_.get(); }

private:
    GraphicEq::Channel                    channel_;
    int                                   fifo_index_;
    Fifo< juce::AudioBuffer< float >, 2 > audio_buffer_fifo_;
    juce::AudioBuffer< float >            buffer_to_fill_;
    juce::Atomic< bool >                  prepared_;
    juce::Atomic< int >                   size_;
};
