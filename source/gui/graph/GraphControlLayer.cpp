#include "GraphControlLayer.h"
#include "ParametricEqProcessor.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphControlLayer::GraphControlLayer(ParametricEqProcessor& processor_ref)
    : processor_ref_(processor_ref)
    , parameters_changed_(false)
    , latched_node_index_(LATCHED_NODE_INDEX_UNSET)
{
    initialiseCoefficientsRefs();
    resetUpdateFlags();

    auto& apvts = processor_ref_.getApvts();

    for (size_t i = 0; i < nodes_.size(); ++i) {
        const auto& filter_id       = static_cast< ParametricEq::FILTER_ID >(i);
        const auto& band_parameters = ParametricEq::PARAMETER_MAP.at(filter_id);

        nodes_.at(i) = std::make_unique< GraphNode >(apvts, band_parameters.enabled_);

        addAndMakeVisible(*nodes_.at(i));

        // Map the x coordinate to the band frequency.
        x_values_.at(i).referTo(apvts.getParameterAsValue(band_parameters.frequency_));

        // Map the y coordinate to the band gain except for the cut filters.
        // Up/down drag on the cut filters currently does nothing.
        if (filter_id != ParametricEq::LOW_CUT && filter_id != ParametricEq::HIGH_CUT) {
            y_values_.at(i).referTo(apvts.getParameterAsValue(band_parameters.gain_));
        }
    }

    ParametricEq::addListenerToAllParameters(processor_ref_.getApvts(), this);
    updateFilterChain();
    startTimerHz(60);
}

/*---------------------------------------------------------------------------
**
*/
GraphControlLayer::~GraphControlLayer()
{
    stopTimer();
    ParametricEq::removeListenerFromAllParameters(processor_ref_.getApvts(), this);
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

    // Plot the paths for each band.
    // These need to be closed paths starting and ending at 0 for the fill effect to look correct.
    g.setColour(Theme::RESPONSE_CURVE_FILL);

    for (size_t band_index = 0; band_index < paths_.size() - 1; ++band_index) {
        auto&       path = paths_.at(band_index);
        const auto& mags = magnitudes_.at(band_index);

        path.startNewSubPath(bounds_x, getPathYCoordinate(0.0, bounds_y, bounds_bottom));

        for (int i = 1; i < static_cast< int >(bounds_width) - 1; ++i) {
            const auto x = bounds_x + static_cast< float >(i);
            const auto y = getPathYCoordinate(mags.at(static_cast< size_t >(i)), bounds_y, bounds_bottom);

            path.lineTo(x, y);
        }

        path.lineTo(bounds_width - 1.f, getPathYCoordinate(0.0, bounds_y, bounds_bottom));
        path.closeSubPath();

        g.fillPath(path);
    }

    // Plot the path for total filter response.
    // This should NOT be closed so that it's just a single line.
    auto&       final_path = paths_.back();
    const auto& final_mags = magnitudes_.back();

    final_path.startNewSubPath(bounds_x, getPathYCoordinate(final_mags.front(), bounds_y, bounds_bottom));

    for (int i = 1; i < static_cast< int >(bounds_width); ++i) {
        const auto x = bounds_x + static_cast< float >(i);
        const auto y = getPathYCoordinate(final_mags.at(static_cast< size_t >(i)), bounds_y, bounds_bottom);

        final_path.lineTo(x, y);
    }

    g.setColour(Theme::RESPONSE_CURVE_AVG);
    g.strokePath(final_path, juce::PathStrokeType(1.f));

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

        const auto& filter_id = static_cast< ParametricEq::FILTER_ID >(i);
        const int   node_x    = getNodeXCoordinate(filter_id, bounds_width);
        double      magnitude = 0.0;

        if (filter_id != ParametricEq::LOW_CUT && filter_id != ParametricEq::HIGH_CUT) {
            const auto mag_it = static_cast< size_t >(node_x) - static_cast< size_t >(bounds_x);
            magnitude         = magnitudes_.at(i).at(mag_it);
        }

        const int node_y = getNodeYCoordinate(magnitude, bounds_y, bounds_bottom);

        node->setBounds(node_x - GraphNode::RADIUS, node_y - GraphNode::RADIUS, GraphNode::DIAMETER, GraphNode::DIAMETER);
        node->coordinates().setXY(node_x, node_y);
    }

    // Vertical lines on both ends of the container to hide the edge of the response curve.
    g.setColour(Theme::MAIN_BACKGROUND);
    constexpr auto line_thickness = 3.f;

    g.drawLine(0.f, 0.f, 0.f, bounds.getBottom(), line_thickness);
    g.drawLine(bounds_width - 1.f, 0.f, bounds_width - 1.f, bounds.getBottom(), line_thickness);
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
    const auto x = static_cast< int >(std::round(event.position.getX()));
    const auto y = static_cast< int >(std::round(event.position.getY()));

    const auto& bounds        = getLocalBounds();
    const auto  bounds_width  = bounds.getWidth();
    const auto  bounds_height = bounds.getHeight();

    // Check that the coordinates are within the legal range.
    if (x < 0 || x > bounds_width || y < 0 || y > bounds_height) {
        return;
    }

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
    const auto hz = mapToLog10< float >(static_cast< float >(x) / static_cast< float >(bounds_width),
                                        ParametricEq::MIN_HZ,
                                        ParametricEq::MAX_HZ);

    const auto db = juce::jmap< float >(static_cast< float >(y),
                                        0.f,
                                        static_cast< float >(bounds_height),
                                        ParametricEq::MAX_DB,
                                        ParametricEq::MIN_DB);

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

    for (auto i = 0; i < ParametricEq::NUM_FILTERS; ++i) {
        const auto filter_id                                 = static_cast< ParametricEq::FILTER_ID >(i);
        const auto [enabled_, frequency_, gain_, q_, slope_] = ParametricEq::PARAMETER_MAP.at(filter_id);

        bool is_match = false;

        is_match |= (enabled_ != ParametricEq::PARAMETER_NOT_SUPPORTED && parameter_id == enabled_);
        is_match |= (frequency_ != ParametricEq::PARAMETER_NOT_SUPPORTED && parameter_id == frequency_);
        is_match |= (gain_ != ParametricEq::PARAMETER_NOT_SUPPORTED && parameter_id == gain_);
        is_match |= (q_ != ParametricEq::PARAMETER_NOT_SUPPORTED && parameter_id == q_);
        is_match |= (slope_ != ParametricEq::PARAMETER_NOT_SUPPORTED && parameter_id == slope_);

        if (is_match) {
            update_flags_.at(filter_id).set(true);
            parameters_changed_.set(true);
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
    if (parameters_changed_.compareAndSetBool(false, true)) {
        updateFilterChain();

        for (size_t i = 0; i < update_flags_.size(); ++i) {
            if (update_flags_.at(i).get()) {
                calculateFilterMagnitudeResponse(static_cast< ParametricEq::FILTER_ID >(i));
            }
        }

        calculateTotalMagnitudeResponse();
        resetUpdateFlags();
        repaint();
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
GraphControlLayer::calculateFilterMagnitudeResponse(const ParametricEq::FILTER_ID& filter_id)
{
    using namespace ParametricEq;

    const auto filter_id_t = static_cast< size_t >(filter_id);
    auto&      filter_mags = magnitudes_.at(filter_id_t);

    if (!processor_ref_.getFilterEnabled(filter_id)) {
        std::fill(filter_mags.begin(), filter_mags.end(), juce::Decibels::gainToDecibels(1.0));
        return;
    }

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

        for (size_t filter = 0; filter < static_cast< size_t >(ParametricEq::NUM_FILTERS); ++filter) {
            mag_all *= juce::Decibels::decibelsToGain(magnitudes_.at(filter).at(px));
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
    return juce::mapToLog10< double >(static_cast< double >(px) / max_px, ParametricEq::MIN_HZ, ParametricEq::MAX_HZ);
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ float
GraphControlLayer::getPathYCoordinate(const double magnitude, const float bounds_top, const float bounds_bottom)
{
    return juce::jmap< float >(static_cast< float >(magnitude),
                               ParametricEq::MIN_DB,
                               ParametricEq::MAX_DB,
                               bounds_bottom,
                               bounds_top);
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ int
GraphControlLayer::getNodeYCoordinate(const double magnitude, const float bounds_top, const float bounds_bottom)
{
    const auto mapped = getPathYCoordinate(magnitude, bounds_top, bounds_bottom);

    return static_cast< int >(std::round(mapped));  //! Round, don't truncate!
}

/*---------------------------------------------------------------------------
**
*/
int
GraphControlLayer::getNodeXCoordinate(const ParametricEq::FILTER_ID& filter_id, const float bounds_width) const
{
    const auto hz      = processor_ref_.getFilterFrequency(filter_id);
    const auto norm_hz = juce::mapFromLog10< float >(hz, ParametricEq::MIN_HZ, ParametricEq::MAX_HZ);
    const auto x       = static_cast< int >(std::round(bounds_width * norm_hz));  //! Round, don't truncate!

    // Clamp the value to keep the node within the graph bounds.
    return juce::jlimit< int >(GraphNode::RADIUS, static_cast< int >(bounds_width) - GraphNode::RADIUS, x);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::resetUpdateFlags()
{
    for (auto& flag : update_flags_) {
        flag.set(false);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::initialiseCoefficientsRefs()
{
    using namespace ParametricEq;

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
