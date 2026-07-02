#include "EqBandWidget.h"
#include "ParametricEqGlobals.h"
#include "ParametricEqTheme.h"

/*---------------------------------------------------------------------------
**
*/
EqBandWidget::EqBandWidget(juce::AudioProcessorValueTreeState& apvts,
                           const parametric_eq::FILTER_ID&     filter_id,
                           const BAND_TYPE&                    band_type)
    : filter_id_(filter_id)
    , is_active_band_(false)
    , band_enablement_button_(apvts, filter_id)
{
    addAndMakeVisible(band_enablement_button_);

    const auto& parameters_ids = parametric_eq::PARAMETER_MAP.at(filter_id);

    juce::String slider_1_parameter_id;
    juce::String slider_2_parameter_id;
    juce::String slider_3_parameter_id;

    switch (band_type) {
    case CUT_BAND:
        slider_1_parameter_id = parameters_ids.frequency_;
        slider_2_parameter_id = parameters_ids.slope_;
        // Slider 3 is unused.
        break;

    case SHELF_BAND:
        slider_1_parameter_id = parameters_ids.frequency_;
        slider_2_parameter_id = parameters_ids.gain_;
        // Slider 3 is unused.
        break;

    case PEAK_BAND:
        slider_1_parameter_id = parameters_ids.frequency_;
        slider_2_parameter_id = parameters_ids.gain_;
        slider_3_parameter_id = parameters_ids.q_;
        break;

    default:
        break;
    }

    if (slider_1_parameter_id.isNotEmpty()) {
        slider_1_ = std::make_unique< TextOnlyRotary >(apvts, slider_1_parameter_id);
        addAndMakeVisible(slider_1_.get());
    }

    if (slider_2_parameter_id.isNotEmpty()) {
        slider_2_ = std::make_unique< TextOnlyRotary >(apvts, slider_2_parameter_id);
        addAndMakeVisible(slider_2_.get());
    }

    if (slider_3_parameter_id.isNotEmpty()) {
        slider_3_ = std::make_unique< TextOnlyRotary >(apvts, slider_3_parameter_id);
        addAndMakeVisible(slider_3_.get());
    }

    band_enablement_button_.onClick = [this]() {
        setEnablement();
        repaint();
    };

    setEnablement();
}

/*---------------------------------------------------------------------------
**
*/
void
EqBandWidget::paint(juce::Graphics& g)
{
    if (band_enablement_button_.getToggleState() && is_active_band_) {
        g.setColour(parametric_eq::theme::ACTIVE_BAND_BACKGROUND);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), parametric_eq::theme::BORDER_RADIUS);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
EqBandWidget::resized()
{
    using Track = juce::Grid::TrackInfo;
    using Fr    = juce::Grid::Fr;

    juce::Grid grid;

    grid.autoColumns = Track(Fr(100));

    grid.templateRows = {
        Track(Fr(25)),  //! Enablement button.
        Track(Fr(25)),  //! Slider 1.
        Track(Fr(25)),  //! Slider 2.
        Track(Fr(25))   //! Slider 3.
    };

    grid.items.add(juce::GridItem(band_enablement_button_));

    if (slider_1_ != nullptr) {
        grid.items.add(juce::GridItem(slider_1_.get()));
    }

    if (slider_2_ != nullptr) {
        grid.items.add(juce::GridItem(slider_2_.get()));
    }

    if (slider_3_ != nullptr) {
        grid.items.add(juce::GridItem(slider_3_.get()));
    }

    grid.performLayout(getLocalBounds());
}

/*---------------------------------------------------------------------------
**
*/
void
EqBandWidget::setIsActiveBand(const bool is_active)
{
    is_active_band_ = is_active;

    repaint();
}

/*---------------------------------------------------------------------------
**
*/
void
EqBandWidget::setEnablement() const
{
    const bool enabled = band_enablement_button_.getToggleState();

    if (slider_1_ != nullptr) {
        slider_1_->setEnablement(enabled);
    }

    if (slider_2_ != nullptr) {
        slider_2_->setEnablement(enabled);
    }

    if (slider_3_ != nullptr) {
        slider_3_->setEnablement(enabled);
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
