#pragma once

#include <JuceHeader.h>

struct PeakBand {
    juce::AudioParameterFloat* peakFreqParam { nullptr };
    juce::AudioParameterFloat* peakGainParam { nullptr };
    juce::AudioParameterFloat* peakQParam { nullptr };
    juce::AudioParameterBool*  peakEnabledParam { nullptr };
};
