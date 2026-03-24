#pragma once

#include <JuceHeader.h>

#include "MonoBufferFifo.h"
#include "ResponseCurve.h"
#include "SpectrumAnalyzer.h"
#include "SpectrumGrid.h"
#include "Theme.h"

class AnalyzerContainer final : public juce::Component
{
public:
    AnalyzerContainer(juce::AudioProcessorValueTreeState&           apvts,
                      double                                        sample_rate,
                      MonoBufferFifo< juce::AudioBuffer< float > >& pre_eq_buffer_l,
                      MonoBufferFifo< juce::AudioBuffer< float > >& pre_eq_buffer_r,
                      MonoBufferFifo< juce::AudioBuffer< float > >& post_eq_buffer_l,
                      MonoBufferFifo< juce::AudioBuffer< float > >& post_eq_buffer_r);

    void paint(juce::Graphics& g) override;
    void resized() override;

    size_t getActiveNodeIndex();

private:
    static void drawDbLabels(const juce::Graphics& g, const juce::Rectangle< int >& bounds);
    static void drawFrequencyLabels(const juce::Graphics& g, const juce::Rectangle< int >& bounds);

    static constexpr int TEXT_WIDTH  = Theme::UI_PADDING * 16;
    static constexpr int DB_INTERVAL = 12;

    static const std::vector< float > HZ_LIST;

    SpectrumGrid     background_grid_;
    SpectrumAnalyzer waveform_pre_eq_;
    SpectrumAnalyzer waveform_post_eq_;
    ResponseCurve    response_curve_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyzerContainer)
};
