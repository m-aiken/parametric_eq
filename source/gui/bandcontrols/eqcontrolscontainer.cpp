#include "EqControlsContainer.h"

//==============================================================================
EqControlsContainer::EqControlsContainer(juce::AudioProcessorValueTreeState& apvts)
{
    lowCutControl  = std::make_unique<CutControl>(apvts, FilterUtils::ChainPositions::LowCut);
    highCutControl = std::make_unique<CutControl>(apvts, FilterUtils::ChainPositions::HighCut);
    addAndMakeVisible(*lowCutControl);
    addAndMakeVisible(*highCutControl);

    for (size_t i = 0; i < peakControls.size(); ++i) {
        peakControls.at(i) = std::make_unique<PeakControl>(apvts, i);
        addAndMakeVisible(*peakControls.at(i));
    }
}

void EqControlsContainer::resized()
{
    if (lowCutControl == nullptr || highCutControl == nullptr) {
        return;
    }

    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    using Fr    = juce::Grid::Fr;

    grid.templateColumns = {
        Track(Fr(1)),
        Track(Fr(1)),
        Track(Fr(1)),
        Track(Fr(1)),
        Track(Fr(1)),
        Track(Fr(1)),
        Track(Fr(1)),
        Track(Fr(1))
    };

    grid.autoRows = Track(Fr(1));

    grid.items.add(juce::GridItem(*lowCutControl));

    for (size_t i = 0; i < peakControls.size(); ++i) {
        if (peakControls.at(i) != nullptr) {
            grid.items.add(juce::GridItem(*peakControls.at(i)));
        }
    }

    grid.items.add(juce::GridItem(*highCutControl));

    grid.setGap(juce::Grid::Px { 4 });

    grid.performLayout(getLocalBounds());
}

void EqControlsContainer::setBandHasNodeSelection(size_t activeNodeIndex)
{
    for (size_t i = 0; i < peakControls.size(); ++i) {
        if (peakControls.at(i) != nullptr) {
            peakControls.at(i)->setNodeIsActive(i == activeNodeIndex);
        }
    }
}
