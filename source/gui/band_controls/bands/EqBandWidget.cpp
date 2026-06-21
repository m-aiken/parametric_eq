#include "EqBandWidget.h"
#include "ParametricEqGlobals.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
EqBandWidget::EqBandWidget(ParametricEqProcessor&          processor_ref,
                           const parametric_eq::FILTER_ID& filter_id,
                           const BAND_TYPE&                band_type)
    : processor_ref_(processor_ref)
    , filter_id_(filter_id)
    , is_active_band_(false)
    , band_enablement_button_(processor_ref, filter_id)
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

    auto& apvts = processor_ref_.getApvts();

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

    apvts.addParameterListener(parametric_eq::PLUGIN_BYPASS_PARAMETER, this);
}

/*---------------------------------------------------------------------------
**
*/
EqBandWidget::~EqBandWidget()
{
    processor_ref_.getApvts().removeParameterListener(parametric_eq::PLUGIN_BYPASS_PARAMETER, this);
}

/*---------------------------------------------------------------------------
**
*/
void
EqBandWidget::paint(juce::Graphics& g)
{
    if (processor_ref_.pluginIsEnabled()) {
        const bool band_enabled = band_enablement_button_.getToggleState();
        g.setColour(is_active_band_ && band_enabled ? parametric_eq::theme::ACTIVE_BAND_BACKGROUND :
                                                      parametric_eq::theme::BAND_BACKGROUND);
    }
    else {
        g.setColour(parametric_eq::theme::PLUGIN_DISABLED_BAND_BACKGROUND);
    }

    g.fillRoundedRectangle(getLocalBounds().toFloat(), parametric_eq::theme::BORDER_RADIUS);
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
EqBandWidget::parameterChanged(const juce::String& parameter_id, float new_value)
{
    juce::ignoreUnused(new_value);

    if (parameter_id == parametric_eq::PLUGIN_BYPASS_PARAMETER) {
        setEnablement();
        repaint();
    }
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
EqBandWidget::setEnablement()
{
    const bool enabled = processor_ref_.pluginIsEnabled() && band_enablement_button_.getToggleState();

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
