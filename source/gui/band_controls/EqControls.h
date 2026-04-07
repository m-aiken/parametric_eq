#pragma once

#include <JuceHeader.h>

#include "CutBandControls.h"
#include "PeakBandControls.h"
#include "ShelfBandControls.h"

class EqControls final : public juce::Component
{
public:
    explicit EqControls(juce::AudioProcessorValueTreeState& apvts);
    void resized() override;

private:
    void setActiveBand(const GraphicEq::FILTER_ID& last_active_band);

    CutBandControls   low_cut_controls_;
    ShelfBandControls low_shelf_controls_;
    PeakBandControls  peak_band_1_;
    PeakBandControls  peak_band_2_;
    PeakBandControls  peak_band_3_;
    PeakBandControls  peak_band_4_;
    PeakBandControls  peak_band_5_;
    ShelfBandControls high_shelf_controls_;
    CutBandControls   high_cut_controls_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqControls)
};
