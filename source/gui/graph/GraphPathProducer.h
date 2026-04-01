#pragma once

#include <JuceHeader.h>

#include "FftInputDataFifo.h"
#include "Fifo.h"
#include "GraphicEqGlobals.h"

class GraphPathProducer final : public juce::Thread
{
public:
    GraphPathProducer(const double sample_rate, FftInputDataFifo& fft_input_data_fifo);
    ~GraphPathProducer() override;

    void run() override;
    void pauseThread();

    void setPathBounds(const juce::Rectangle< float >& bounds);

    int  getNumAvailableForReading() const;
    bool pull(juce::Path& path);

    void setProcessingEnabled(const bool enable);

private:
    void performFft();
    void smoothFftOutput();
    void generatePath();

    std::atomic< bool > processing_enabled_;

    // FFT.
    FftInputDataFifo&                     fft_input_data_fifo_;
    juce::dsp::FFT                        fft_;
    juce::dsp::WindowingFunction< float > windowing_func_;
    juce::AudioBuffer< float >            fft_input_buffer_;
    GraphicEq::FFT::OutputBuffer          fft_output_buffer_;

    // Smoothed FFT output.
    GraphicEq::FFT::OutputBuffer smoothed_fft_output_;
    static constexpr float       DECAY_RATE_DB_PER_SECOND = 30.f;
    static constexpr float       DECAY_RATE               = DECAY_RATE_DB_PER_SECOND / 60.f;

    // Path.
    static constexpr int     PATH_RESOLUTION = 2;
    double                   sample_rate_;
    juce::Rectangle< float > path_bounds_;
    Fifo< juce::Path, 2 >    path_fifo_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphPathProducer)
};
