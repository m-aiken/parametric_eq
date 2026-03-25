#pragma once

#include <JuceHeader.h>

#include "Fifo.h"

class FftDataGenerator
{
public:
    FftDataGenerator();

    void produceFFTDataForRendering(const juce::AudioBuffer< float >& audio_data);
    int  getNumAvailableFFTDataBlocks() const;
    bool getFFTData(std::vector< float >& data);

    enum {
        SIZE_2048 = 11,
        SIZE_4096 = 12,
        SIZE_8192 = 13,
    };

    static constexpr int FFT_ORDER = SIZE_8192;
    static constexpr int FFT_SIZE  = 1 << FFT_ORDER;
    static constexpr int NUM_BINS  = FFT_SIZE / 2;

private:
    juce::dsp::FFT                        forward_fft_;
    juce::dsp::WindowingFunction< float > windowing_func_;
    std::vector< float >                  fft_data_;
    Fifo< std::vector< float >, 2 >       fft_data_fifo_;
};
