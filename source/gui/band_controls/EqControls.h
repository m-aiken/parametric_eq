#pragma once

#include <JuceHeader.h>

#include "CutBandControls.h"
#include "GraphicEqGlobals.h"
#include "PeakBandControls.h"

class EqControls final : public juce::Component
{
public:
    explicit EqControls(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;

    void setBandHasNodeSelection(size_t active_node_index) const;

private:
    CutBandControls low_cut_control_;
    CutBandControls high_cut_control_;

    std::array< std::unique_ptr< PeakBandControls >, GraphicEq::NUM_PEAK_BANDS > peak_controls_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqControls)
};
