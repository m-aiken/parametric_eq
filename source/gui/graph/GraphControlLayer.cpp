#include "GraphControlLayer.h"
#include "ParametricEqProcessor.h"
#include "ParametricEqTheme.h"

/*---------------------------------------------------------------------------
**
*/
GraphControlLayer::GraphControlLayer(ParametricEqProcessor& processor_ref)
    : processor_ref_(processor_ref)
    , filter_parameters_changed_(true)
    , latched_node_index_(LATCHED_NODE_INDEX_UNSET)
{
    initialiseCoefficientsRefs();
    processor_ref_.updateFilterChain(filter_chain_, true);

    auto& apvts = processor_ref_.getApvts();

    for (size_t i = 0; i < nodes_.size(); ++i) {
        const auto& filter_id       = static_cast< parametric_eq::FILTER_ID >(i);
        const auto& band_parameters = parametric_eq::PARAMETER_MAP.at(filter_id);

        nodes_.at(i) = std::make_unique< GraphNode >(apvts, band_parameters.enabled_);

        addAndMakeVisible(*nodes_.at(i));

        // Map the x coordinate to the band frequency.
        x_values_.at(i).referTo(apvts.getParameterAsValue(band_parameters.frequency_));

        // Map the y coordinate to the band gain except for the cut filters.
        // Up/down drag on the cut filters currently does nothing.
        if (filter_id != parametric_eq::LOW_CUT && filter_id != parametric_eq::HIGH_CUT) {
            y_values_.at(i).referTo(apvts.getParameterAsValue(band_parameters.gain_));
        }
    }

    parametric_eq::addListenerToAllParameters(processor_ref_.getApvts(), this);
    resetUpdateFlags(true);
    startTimerHz(60);
}

/*---------------------------------------------------------------------------
**
*/
GraphControlLayer::~GraphControlLayer()
{
    stopTimer();
    parametric_eq::removeListenerFromAllParameters(processor_ref_.getApvts(), this);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::paint(juce::Graphics& g)
{
    const auto& bounds        = getLocalBounds().toFloat();
    const auto  bounds_width  = bounds.getWidth();
    const auto  bounds_x      = bounds.getX();
    const auto  bounds_y      = bounds.getY();
    const auto  bounds_bottom = bounds.getBottom();

    for (auto& path : paths_) {
        path.clear();
    }

    // Gradient fill for the band paths.
    const auto& fill_enabled  = parametric_eq::theme::RESPONSE_CURVE_FILL;
    const auto& fill_disabled = parametric_eq::theme::RESPONSE_CURVE_DISABLED;

    auto enabled_gradient = juce::ColourGradient::vertical(fill_enabled, bounds_bottom, fill_enabled, bounds_y);
    enabled_gradient.addColour(0.5, fill_enabled.withAlpha(0.1f));

    auto disabled_gradient = juce::ColourGradient::vertical(fill_disabled, bounds_bottom, fill_disabled, bounds_y);
    disabled_gradient.addColour(0.5, fill_disabled.withAlpha(0.025f));

    // Plot the paths for each band.
    // These need to be closed paths starting and ending at 0 for the fill effect to look correct.
    for (size_t band_index = 0; band_index < paths_.size() - 1; ++band_index) {
        auto&       path = paths_.at(band_index);
        const auto& mags = magnitudes_.at(band_index);

        path.startNewSubPath(bounds_x, getPathYCoordinate(0.0, bounds_bottom));

        for (int i = 1; i < static_cast< int >(bounds_width) - 1; ++i) {
            const auto x = bounds_x + static_cast< float >(i);
            const auto y = getPathYCoordinate(mags.at(static_cast< size_t >(i)), bounds_bottom);

            path.lineTo(x, y);
        }

        path.lineTo(bounds_width - 1.f, getPathYCoordinate(0.0, bounds_bottom));
        path.closeSubPath();

        const auto band_enabled = processor_ref_.getFilterEnabled(static_cast< parametric_eq::FILTER_ID >(band_index));
        g.setGradientFill(band_enabled ? enabled_gradient : disabled_gradient);
        g.fillPath(path);
    }

    // Plot the path for total filter response.
    // This should NOT be closed so that it's just a single line.
    auto&       final_path = paths_.back();
    const auto& final_mags = magnitudes_.back();

    final_path.startNewSubPath(bounds_x, getPathYCoordinate(final_mags.front(), bounds_bottom));

    for (int i = 1; i < static_cast< int >(bounds_width); ++i) {
        const auto x = bounds_x + static_cast< float >(i);
        const auto y = getPathYCoordinate(final_mags.at(static_cast< size_t >(i)), bounds_bottom);

        final_path.lineTo(x, y);
    }

    g.setColour(parametric_eq::theme::RESPONSE_CURVE_AVG);
    g.strokePath(final_path, juce::PathStrokeType(2.f));

    // Draw the nodes.
    for (size_t i = 0; i < nodes_.size(); ++i) {
        auto& node = nodes_.at(i);

        if (node == nullptr) {
            continue;
        }

        // Low cut / high cut:
        // x coordinate = correct frequency.
        // y coordinate = fixed at 0dB.
        // All other bands:
        // x coordinate = correct frequency.
        // y coordinate = correct magnitude.

        const auto& filter_id = static_cast< parametric_eq::FILTER_ID >(i);
        const auto  node_x    = getNodeXCoordinate(filter_id, bounds_width);
        const auto  node_y    = getNodeYCoordinate(filter_id, bounds_bottom);

        const auto node_bounds = juce::Rectangle< float >(node_x - GraphNode::RADIUS,
                                                          node_y - GraphNode::RADIUS,
                                                          GraphNode::DIAMETER,
                                                          GraphNode::DIAMETER)
                                     .toNearestInt();

        node->setBounds(node_bounds);
        node->coordinates().setXY(node_x, node_y);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::resized()
{
    const auto bounds_width = static_cast< size_t >(getLocalBounds().getWidth());

    for (auto& mag : magnitudes_) {
        mag.resize(bounds_width);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::mouseDrag(const juce::MouseEvent& event)
{
    if (!processor_ref_.pluginIsEnabled()) {
        return;
    }

    const auto& bounds        = getLocalBounds().toFloat();
    const auto  bounds_width  = bounds.getWidth();
    const auto  bounds_height = bounds.getHeight();

    const auto x = constrainXCoordinate(event.position.getX(), bounds_width);
    const auto y = constrainYCoordinate(event.position.getY(), bounds_height);

    // If the latched dragged node is unset calculate the closest node.
    if (latched_node_index_ == LATCHED_NODE_INDEX_UNSET) {
        auto shortest_distance = bounds_width;

        for (size_t i = 0; i < nodes_.size(); ++i) {
            const auto& node = nodes_.at(i);

            if (node == nullptr) {
                continue;
            }

            // Only check nodes that are enabled.
            if (!node->getToggleState()) {
                continue;
            }

            const auto& node_xy  = node->coordinates();
            const auto  distance = std::abs(node_xy.getX() - x) + std::abs(node_xy.getY() - y);

            if (distance < shortest_distance && distance <= DRAG_PROXIMITY_LIMIT) {
                latched_node_index_ = i;
                shortest_distance   = distance;
            }
        }
    }

    if (latched_node_index_ == LATCHED_NODE_INDEX_UNSET) {
        return;
    }

    // Update the frequency and gain parameters for the node being dragged.
    const auto hz = mapToLog10< float >(x / bounds_width, parametric_eq::MIN_HZ, parametric_eq::MAX_HZ);
    const auto db = juce::jmap< float >(y, 0.f, bounds_height, parametric_eq::MAX_DB, parametric_eq::MIN_DB);

    x_values_.at(latched_node_index_) = hz;
    y_values_.at(latched_node_index_) = db;
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::mouseUp(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);

    // Reset latched dragged node reference.
    latched_node_index_ = LATCHED_NODE_INDEX_UNSET;
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::parameterChanged(const juce::String& parameter_id, float new_value)
{
    juce::ignoreUnused(new_value);

    for (auto i = 0; i < parametric_eq::NUM_FILTERS; ++i) {
        const auto filter_id                                 = static_cast< parametric_eq::FILTER_ID >(i);
        const auto [enabled_, frequency_, gain_, q_, slope_] = parametric_eq::PARAMETER_MAP.at(filter_id);

        bool is_match = false;

        is_match |= (enabled_ != parametric_eq::PARAMETER_NOT_SUPPORTED && parameter_id == enabled_);
        is_match |= (frequency_ != parametric_eq::PARAMETER_NOT_SUPPORTED && parameter_id == frequency_);
        is_match |= (gain_ != parametric_eq::PARAMETER_NOT_SUPPORTED && parameter_id == gain_);
        is_match |= (q_ != parametric_eq::PARAMETER_NOT_SUPPORTED && parameter_id == q_);
        is_match |= (slope_ != parametric_eq::PARAMETER_NOT_SUPPORTED && parameter_id == slope_);

        if (is_match) {
            update_flags_.at(filter_id).set(true);
            filter_parameters_changed_.set(true);
            break;
        }
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::timerCallback()
{
    bool should_repaint = false;

    if (filter_parameters_changed_.compareAndSetBool(false, true)) {
        updateFilterChain();

        for (size_t i = 0; i < update_flags_.size(); ++i) {
            if (update_flags_.at(i).get()) {
                calculateFilterMagnitudeResponse(static_cast< parametric_eq::FILTER_ID >(i));
            }
        }

        calculateTotalMagnitudeResponse();
        resetUpdateFlags();
        should_repaint = true;
    }

    if (should_repaint) {
        repaint();
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::setActiveBand(const parametric_eq::FILTER_ID& active_band) const
{
    for (size_t i = 0; i < nodes_.size(); ++i) {
        nodes_.at(i)->setIsActiveBand(static_cast< size_t >(active_band) == i);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::updateFilterChain()
{
    processor_ref_.updateFilterChain(filter_chain_);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::calculateFilterMagnitudeResponse(const parametric_eq::FILTER_ID& filter_id)
{
    using namespace parametric_eq;

    const auto filter_id_t  = static_cast< size_t >(filter_id);
    auto&      filter_mags  = magnitudes_.at(filter_id_t);
    const int  bounds_width = getLocalBounds().getWidth();
    const auto sample_rate  = processor_ref_.getSampleRate();

    if (filter_id == LOW_CUT || filter_id == HIGH_CUT) {
        const auto& filter = filter_id == LOW_CUT ? filter_chain_.get< LOW_CUT >() : filter_chain_.get< HIGH_CUT >();
        const auto& coefficients = filter_id == LOW_CUT ? lcut_coefficients_ : hcut_coefficients_;

        for (auto& c : coefficients) {
            if (c == nullptr) {
                return;
            }
        }

        for (auto i = 0; i < bounds_width; ++i) {
            const auto hz        = getHzFromPx(i, bounds_width);
            auto       mag_at_hz = 1.0;

            if (!filter.isBypassed< SLOPE_12_DBPO >()) {
                mag_at_hz *= coefficients[SLOPE_12_DBPO]->getMagnitudeForFrequency(hz, sample_rate);
            }
            if (!filter.isBypassed< SLOPE_24_DBPO >()) {
                mag_at_hz *= coefficients[SLOPE_24_DBPO]->getMagnitudeForFrequency(hz, sample_rate);
            }
            if (!filter.isBypassed< SLOPE_36_DBPO >()) {
                mag_at_hz *= coefficients[SLOPE_36_DBPO]->getMagnitudeForFrequency(hz, sample_rate);
            }
            if (!filter.isBypassed< SLOPE_48_DBPO >()) {
                mag_at_hz *= coefficients[SLOPE_48_DBPO]->getMagnitudeForFrequency(hz, sample_rate);
            }

            filter_mags.at(static_cast< size_t >(i)) = juce::Decibels::gainToDecibels(mag_at_hz);
        }
    }
    else {
        if (band_coefficients_.at(filter_id) == nullptr) {
            return;
        }

        for (auto i = 0; i < bounds_width; ++i) {
            const auto hz        = getHzFromPx(i, bounds_width);
            const auto mag_at_hz = band_coefficients_.at(filter_id_t)->getMagnitudeForFrequency(hz, sample_rate);

            filter_mags.at(static_cast< size_t >(i)) = juce::Decibels::gainToDecibels(mag_at_hz);
        }
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::calculateTotalMagnitudeResponse()
{
    const auto bounds_width = static_cast< size_t >(getLocalBounds().getWidth());

    // Calculate the total magnitude across all filters.
    for (size_t px = 0; px < bounds_width; ++px) {
        double mag_all = 1.0;

        for (size_t filter = 0; filter < static_cast< size_t >(parametric_eq::NUM_FILTERS); ++filter) {
            // Only include the filter if it's enabled.
            if (processor_ref_.getFilterEnabled(static_cast< parametric_eq::FILTER_ID >(filter))) {
                mag_all *= juce::Decibels::decibelsToGain(magnitudes_.at(filter).at(px));
            }
        }

        magnitudes_.back().at(px) = juce::Decibels::gainToDecibels(mag_all);
    }
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ double
GraphControlLayer::getHzFromPx(const int px, const int max_px)
{
    return juce::mapToLog10< double >(static_cast< double >(px) / max_px, parametric_eq::MIN_HZ, parametric_eq::MAX_HZ);
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ float
GraphControlLayer::getPathYCoordinate(const double magnitude, const float bounds_bottom)
{
    return juce::jmap< float >(static_cast< float >(magnitude),
                               parametric_eq::MIN_DB,
                               parametric_eq::MAX_DB,
                               bounds_bottom,
                               0);
}

/*---------------------------------------------------------------------------
**
*/
float
GraphControlLayer::getNodeYCoordinate(const parametric_eq::FILTER_ID& filter_id, const float bounds_bottom) const
{
    const auto mag = static_cast< double >(processor_ref_.getFilterDecibels(filter_id));
    const auto y   = getPathYCoordinate(mag, bounds_bottom);

    return constrainYCoordinate(y, bounds_bottom);
}

/*---------------------------------------------------------------------------
**
*/
float
GraphControlLayer::getNodeXCoordinate(const parametric_eq::FILTER_ID& filter_id, const float bounds_width) const
{
    const auto hz      = processor_ref_.getFilterFrequency(filter_id);
    const auto norm_hz = juce::mapFromLog10< float >(hz, parametric_eq::MIN_HZ, parametric_eq::MAX_HZ);
    const auto x       = bounds_width * norm_hz;

    return constrainXCoordinate(x, bounds_width);
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ float
GraphControlLayer::constrainYCoordinate(const float y, const float bounds_bottom)
{
    // Clamp the value to keep the node within the graph bounds.
    return juce::jlimit< float >(0, bounds_bottom, y);
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ float
GraphControlLayer::constrainXCoordinate(const float x, const float bounds_width)
{
    // Clamp the value to keep the node within the graph bounds.
    return juce::jlimit< float >(0, bounds_width, x);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::resetUpdateFlags(const bool should_update)
{
    for (auto& flag : update_flags_) {
        flag.set(should_update);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::initialiseCoefficientsRefs()
{
    using namespace parametric_eq;

    lcut_coefficients_[SLOPE_12_DBPO] = filter_chain_.get< LOW_CUT >().get< SLOPE_12_DBPO >().coefficients;
    lcut_coefficients_[SLOPE_24_DBPO] = filter_chain_.get< LOW_CUT >().get< SLOPE_24_DBPO >().coefficients;
    lcut_coefficients_[SLOPE_36_DBPO] = filter_chain_.get< LOW_CUT >().get< SLOPE_36_DBPO >().coefficients;
    lcut_coefficients_[SLOPE_48_DBPO] = filter_chain_.get< LOW_CUT >().get< SLOPE_48_DBPO >().coefficients;
    hcut_coefficients_[SLOPE_12_DBPO] = filter_chain_.get< HIGH_CUT >().get< SLOPE_12_DBPO >().coefficients;
    hcut_coefficients_[SLOPE_24_DBPO] = filter_chain_.get< HIGH_CUT >().get< SLOPE_24_DBPO >().coefficients;
    hcut_coefficients_[SLOPE_36_DBPO] = filter_chain_.get< HIGH_CUT >().get< SLOPE_36_DBPO >().coefficients;
    hcut_coefficients_[SLOPE_48_DBPO] = filter_chain_.get< HIGH_CUT >().get< SLOPE_48_DBPO >().coefficients;
    band_coefficients_[LOW_CUT]       = nullptr;
    band_coefficients_[LOW_SHELF]     = filter_chain_.get< LOW_SHELF >().coefficients;
    band_coefficients_[PEAK_BAND_1]   = filter_chain_.get< PEAK_BAND_1 >().coefficients;
    band_coefficients_[PEAK_BAND_2]   = filter_chain_.get< PEAK_BAND_2 >().coefficients;
    band_coefficients_[PEAK_BAND_3]   = filter_chain_.get< PEAK_BAND_3 >().coefficients;
    band_coefficients_[PEAK_BAND_4]   = filter_chain_.get< PEAK_BAND_4 >().coefficients;
    band_coefficients_[PEAK_BAND_5]   = filter_chain_.get< PEAK_BAND_5 >().coefficients;
    band_coefficients_[HIGH_SHELF]    = filter_chain_.get< HIGH_SHELF >().coefficients;
    band_coefficients_[HIGH_CUT]      = nullptr;
}

/*---------------------------------------------------------------------------
** End of File
*/
