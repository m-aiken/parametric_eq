#pragma once

#include <JuceHeader.h>

class FftInputDataFifo
{
public:
    FftInputDataFifo();

    void pushData(const juce::AudioBuffer< float >& input_buffer);
    bool pullData(juce::AudioBuffer< float >& buffer_to_fill) const;

private:
    juce::AudioBuffer< float > fifo_;
    juce::CriticalSection      fifo_critical_section_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FftInputDataFifo)
};
