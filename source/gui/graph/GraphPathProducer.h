#pragma once

#include <JuceHeader.h>

#include "AnalyzerPathGenerator.h"
#include "FftDataGenerator.h"
#include "MonoBufferFifo.h"

class GraphPathProducer final : public juce::Thread
{
public:
    GraphPathProducer(const double sample_rate, MonoBufferFifo& scsf);
    ~GraphPathProducer() override;

    void run() override;

    double getBinWidth() const;
    void   pauseThread();
    void   setFFTRectBounds(const juce::Rectangle< float >& bounds);

    bool pull(juce::Path& path);
    int  getNumAvailableForReading() const;
    void toggleProcessing(const bool toggle_state);

private:
    void updateRenderData(const std::vector< float >& fft_data);

    MonoBufferFifo& single_channel_sample_fifo_;

    FftDataGenerator           fft_data_generator_;
    AnalyzerPathGenerator      path_generator_;
    std::vector< float >       render_data_;
    juce::AudioBuffer< float > buffer_for_generator_;
    double                     sample_rate_;
    juce::Rectangle< float >   fft_bounds_;

    std::atomic< bool > processing_is_enabled_ { false };

    static constexpr float DECAY_RATE_DB_PER_SECOND = 30.f;
    static constexpr float DECAY_RATE               = DECAY_RATE_DB_PER_SECOND / 60.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphPathProducer)
};
