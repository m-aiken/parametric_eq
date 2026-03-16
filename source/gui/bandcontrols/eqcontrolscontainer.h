#pragma once

#include <JuceHeader.h>

#include "CutControl.h"
#include "GraphicEqGlobals.h"
#include "PeakControl.h"

struct EqControlsContainer : juce::Component {
    EqControlsContainer(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;

    void setBandHasNodeSelection(size_t activeNodeIndex);

private:
    std::unique_ptr< CutControl > lowCutControl;
    std::unique_ptr< CutControl > highCutControl;

    std::array< std::unique_ptr< PeakControl >, GraphicEqGlobals::getNumPeakBands() > peakControls;
};
