#pragma once

#include <JuceHeader.h>

#include "DisableButton.h"
#include "ParametricEqProcessor.h"
#include "WaveformButton.h"

class MenuBar final : public juce::Component
{
public:
    explicit MenuBar(ParametricEqProcessor& processor_ref);

    void resized() override;

private:
    DisableButton  disable_button_;
    WaveformButton waveform_button_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBar)
};
