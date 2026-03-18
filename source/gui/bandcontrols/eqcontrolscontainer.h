#pragma once

#include <JuceHeader.h>

#include "CutControl.h"
#include "GraphicEqGlobals.h"
#include "PeakControl.h"

class EqControlsContainer final : public juce::Component
{
public:
    explicit EqControlsContainer(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;

    void setBandHasNodeSelection(size_t activeNodeIndex);

private:
    CutControl lowCutControl;
    CutControl highCutControl;

    std::array< std::unique_ptr< PeakControl >, GraphicEq::NUM_PEAK_BANDS > peakControls;
};
