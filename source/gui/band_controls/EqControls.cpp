#include "EqControls.h"

/*---------------------------------------------------------------------------
**
*/
EqControls::EqControls(juce::AudioProcessorValueTreeState& apvts)
    : low_cut_control_(apvts, FilterUtils::ChainPositions::LowCut)
    , high_cut_control_(apvts, FilterUtils::ChainPositions::HighCut)
{
    addAndMakeVisible(low_cut_control_);
    addAndMakeVisible(high_cut_control_);

    for (size_t i = 0; i < peak_controls_.size(); ++i) {
        peak_controls_.at(i) = std::make_unique< PeakBandControls >(apvts, i);
        addAndMakeVisible(*peak_controls_.at(i));
    }
}

/*---------------------------------------------------------------------------
**
*/
void
EqControls::resized()
{
    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    using Fr    = juce::Grid::Fr;

    grid.templateColumns = {
        Track(Fr(1)),  //! Low cut.
        Track(Fr(1)),  //! Band 1.
        Track(Fr(1)),  //! Band 2.
        Track(Fr(1)),  //! Band 3.
        Track(Fr(1)),  //! Band 4.
        Track(Fr(1)),  //! Band 5.
        Track(Fr(1)),  //! Band 6.
        Track(Fr(1)),  //! High cut.
    };

    grid.autoRows = Track(Fr(1));

    grid.items.add(juce::GridItem(low_cut_control_));

    for (size_t i = 0; i < peak_controls_.size(); ++i) {
        if (peak_controls_.at(i) != nullptr) {
            grid.items.add(juce::GridItem(*peak_controls_.at(i)));
        }
    }

    grid.items.add(juce::GridItem(high_cut_control_));

    grid.setGap(juce::Grid::Px(4));

    grid.performLayout(getLocalBounds());
}

/*---------------------------------------------------------------------------
**
*/
void
EqControls::setBandHasNodeSelection(size_t active_node_index) const
{
    for (size_t i = 0; i < peak_controls_.size(); ++i) {
        if (peak_controls_.at(i) != nullptr) {
            peak_controls_.at(i)->setNodeIsActive(i == active_node_index);
        }
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
