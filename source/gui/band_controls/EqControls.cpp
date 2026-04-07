#include "EqControls.h"
#include "GraphicEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
EqControls::EqControls(juce::AudioProcessorValueTreeState& apvts)
    : low_cut_controls_(apvts, GraphicEq::LOW_CUT, [this]() { setActiveBand(GraphicEq::LOW_CUT); })
    , low_shelf_controls_(apvts, GraphicEq::LOW_SHELF, [this]() { setActiveBand(GraphicEq::LOW_SHELF); })
    , peak_band_1_(apvts, GraphicEq::PEAK_BAND_1, [this]() { setActiveBand(GraphicEq::PEAK_BAND_1); })
    , peak_band_2_(apvts, GraphicEq::PEAK_BAND_2, [this]() { setActiveBand(GraphicEq::PEAK_BAND_2); })
    , peak_band_3_(apvts, GraphicEq::PEAK_BAND_3, [this]() { setActiveBand(GraphicEq::PEAK_BAND_3); })
    , peak_band_4_(apvts, GraphicEq::PEAK_BAND_4, [this]() { setActiveBand(GraphicEq::PEAK_BAND_4); })
    , peak_band_5_(apvts, GraphicEq::PEAK_BAND_5, [this]() { setActiveBand(GraphicEq::PEAK_BAND_5); })
    , high_shelf_controls_(apvts, GraphicEq::HIGH_SHELF, [this]() { setActiveBand(GraphicEq::HIGH_SHELF); })
    , high_cut_controls_(apvts, GraphicEq::HIGH_CUT, [this]() { setActiveBand(GraphicEq::HIGH_CUT); })
{
    addAndMakeVisible(low_cut_controls_);
    addAndMakeVisible(low_shelf_controls_);
    addAndMakeVisible(peak_band_1_);
    addAndMakeVisible(peak_band_2_);
    addAndMakeVisible(peak_band_3_);
    addAndMakeVisible(peak_band_4_);
    addAndMakeVisible(peak_band_5_);
    addAndMakeVisible(high_shelf_controls_);
    addAndMakeVisible(high_cut_controls_);

    setActiveBand(GraphicEq::PEAK_BAND_3);
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
        Track(Fr(1)),  //! Low Cut.
        Track(Fr(1)),  //! Low Shelf.
        Track(Fr(1)),  //! Peak Band 1.
        Track(Fr(1)),  //! Peak Band 2.
        Track(Fr(1)),  //! Peak Band 3.
        Track(Fr(1)),  //! Peak Band 4.
        Track(Fr(1)),  //! Peak Band 5.
        Track(Fr(1)),  //! High Shelf.
        Track(Fr(1)),  //! High Cut.
    };

    grid.autoRows = Track(Fr(1));

    grid.items.add(juce::GridItem(low_cut_controls_));
    grid.items.add(juce::GridItem(low_shelf_controls_));
    grid.items.add(juce::GridItem(peak_band_1_));
    grid.items.add(juce::GridItem(peak_band_2_));
    grid.items.add(juce::GridItem(peak_band_3_));
    grid.items.add(juce::GridItem(peak_band_4_));
    grid.items.add(juce::GridItem(peak_band_5_));
    grid.items.add(juce::GridItem(high_shelf_controls_));
    grid.items.add(juce::GridItem(high_cut_controls_));

    grid.setGap(juce::Grid::Px(Theme::UI_PADDING));

    const auto& bounds = getLocalBounds();
    grid.performLayout(
        bounds.withSizeKeepingCentre(bounds.getWidth() - Theme::UI_PADDING, bounds.getHeight() - Theme::UI_PADDING));
}

/*---------------------------------------------------------------------------
**
*/
void
EqControls::setActiveBand(const GraphicEq::FILTER_ID& last_active_band)
{
    low_cut_controls_.setIsLastActiveBand(last_active_band == GraphicEq::LOW_CUT);
    low_shelf_controls_.setIsLastActiveBand(last_active_band == GraphicEq::LOW_SHELF);
    peak_band_1_.setIsLastActiveBand(last_active_band == GraphicEq::PEAK_BAND_1);
    peak_band_2_.setIsLastActiveBand(last_active_band == GraphicEq::PEAK_BAND_2);
    peak_band_3_.setIsLastActiveBand(last_active_band == GraphicEq::PEAK_BAND_3);
    peak_band_4_.setIsLastActiveBand(last_active_band == GraphicEq::PEAK_BAND_4);
    peak_band_5_.setIsLastActiveBand(last_active_band == GraphicEq::PEAK_BAND_5);
    high_shelf_controls_.setIsLastActiveBand(last_active_band == GraphicEq::HIGH_SHELF);
    high_cut_controls_.setIsLastActiveBand(last_active_band == GraphicEq::HIGH_CUT);
}

/*---------------------------------------------------------------------------
** End of File
*/
