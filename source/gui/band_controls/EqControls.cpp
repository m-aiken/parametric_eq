#include "EqControls.h"
#include "ParametricEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
EqControls::EqControls(juce::AudioProcessorValueTreeState& apvts)
    : low_cut_controls_(apvts, ParametricEq::LOW_CUT, EqBandWidget::CUT_BAND)
    , low_shelf_controls_(apvts, ParametricEq::LOW_SHELF, EqBandWidget::SHELF_BAND)
    , peak_band_1_(apvts, ParametricEq::PEAK_BAND_1, EqBandWidget::PEAK_BAND)
    , peak_band_2_(apvts, ParametricEq::PEAK_BAND_2, EqBandWidget::PEAK_BAND)
    , peak_band_3_(apvts, ParametricEq::PEAK_BAND_3, EqBandWidget::PEAK_BAND)
    , peak_band_4_(apvts, ParametricEq::PEAK_BAND_4, EqBandWidget::PEAK_BAND)
    , peak_band_5_(apvts, ParametricEq::PEAK_BAND_5, EqBandWidget::PEAK_BAND)
    , high_shelf_controls_(apvts, ParametricEq::HIGH_SHELF, EqBandWidget::SHELF_BAND)
    , high_cut_controls_(apvts, ParametricEq::HIGH_CUT, EqBandWidget::CUT_BAND)
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

    setActiveBand(ParametricEq::NULL_BAND);
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

    constexpr auto margin = Theme::UI_PADDING * 2;
    grid.setGap(juce::Grid::Px(margin));

    const auto& bounds = getLocalBounds();
    grid.performLayout(bounds.withSizeKeepingCentre(bounds.getWidth() - (margin * 2), bounds.getHeight() - (margin * 2)));
}

/*---------------------------------------------------------------------------
**
*/
void
EqControls::setActiveBand(const ParametricEq::FILTER_ID& active_band)
{
    low_cut_controls_.setIsActiveBand(active_band == ParametricEq::LOW_CUT);
    low_shelf_controls_.setIsActiveBand(active_band == ParametricEq::LOW_SHELF);
    peak_band_1_.setIsActiveBand(active_band == ParametricEq::PEAK_BAND_1);
    peak_band_2_.setIsActiveBand(active_band == ParametricEq::PEAK_BAND_2);
    peak_band_3_.setIsActiveBand(active_band == ParametricEq::PEAK_BAND_3);
    peak_band_4_.setIsActiveBand(active_band == ParametricEq::PEAK_BAND_4);
    peak_band_5_.setIsActiveBand(active_band == ParametricEq::PEAK_BAND_5);
    high_shelf_controls_.setIsActiveBand(active_band == ParametricEq::HIGH_SHELF);
    high_cut_controls_.setIsActiveBand(active_band == ParametricEq::HIGH_CUT);
}

/*---------------------------------------------------------------------------
** End of File
*/
