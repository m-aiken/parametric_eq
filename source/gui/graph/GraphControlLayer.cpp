#include "GraphControlLayer.h"
#include "GraphicEqProcessor.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphControlLayer::GraphControlLayer(GraphicEqProcessor& processor_ref)
    : processor_ref_(processor_ref)
    , parameters_changed_(false)
    , active_node_(0)
{
    auto& apvts = processor_ref_.getApvts();

    for (size_t i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        const auto& filter_id            = static_cast< GraphicEq::FILTER_ID >(GraphicEq::PEAK_BAND_1 + i);
        const auto& peak_band_parameters = GraphicEq::PARAMETER_MAP.at(filter_id);

        peak_nodes_.at(i) = std::make_unique< GraphNode >(apvts, peak_band_parameters.enabled_);

        addAndMakeVisible(*peak_nodes_.at(i));

        node_coordinates_.at(i).setXY(0, 0);

        x_values_.at(i).referTo(apvts.getParameterAsValue(peak_band_parameters.frequency_));
        y_values_.at(i).referTo(apvts.getParameterAsValue(peak_band_parameters.gain_));
    }

    addListeners();
    updateFilterChain();
    startTimerHz(60);
}

/*---------------------------------------------------------------------------
**
*/
GraphControlLayer::~GraphControlLayer()
{
    stopTimer();
    removeListeners();
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::paint(juce::Graphics& g)
{
    const auto& bounds_i        = getLocalBounds();
    const auto  bounds_i_width  = bounds_i.getWidth();
    const auto  bounds_i_x      = bounds_i.getX();
    const auto  bounds_i_y      = bounds_i.getY();
    const auto  bounds_i_bottom = bounds_i.getBottom();

    const auto& bounds_f        = getLocalBounds().toFloat();
    const auto  bounds_f_width  = bounds_f.getWidth();
    const auto  bounds_f_x      = bounds_f.getX();
    const auto  bounds_f_y      = bounds_f.getY();
    const auto  bounds_f_bottom = bounds_f.getBottom();

    updateMagnitudes();

    for (auto& path : paths_) {
        path.clear();
    }

    // Plot the paths for each band.
    // These need to be closed paths starting and ending at 0 for the fill effect to look correct.
    for (size_t band_index = 0; band_index < paths_.size() - 1; ++band_index) {
        auto&       path = paths_.at(band_index);
        const auto& mags = magnitudes_.at(band_index);

        path.startNewSubPath(bounds_f_x, getYCoordinateFromMag(0.0, bounds_f_y, bounds_f_bottom));

        for (int i = 1; i < bounds_i_width - 1; ++i) {
            const auto x = bounds_f_x + static_cast< float >(i);
            const auto y = getYCoordinateFromMag(mags.at(i), bounds_f_y, bounds_f_bottom);

            path.lineTo(x, y);
        }

        path.lineTo(bounds_f_width - 1.f, getYCoordinateFromMag(0.0, bounds_f_y, bounds_f_bottom));
        path.closeSubPath();
    }

    // Draw the bolder outline.
    g.setColour(Theme::RESPONSE_CURVE);
    for (size_t path_index = 0; path_index < paths_.size() - 1; ++path_index) {
        g.strokePath(paths_.at(path_index), juce::PathStrokeType(1.f));
    }

    // Draw the faded fill.
    g.setColour(Theme::RESPONSE_CURVE.withAlpha(0.2f));
    for (size_t path_index = 0; path_index < paths_.size() - 1; ++path_index) {
        g.fillPath(paths_.at(path_index));
    }

    // Plot the path for total filter response.
    // This should NOT be closed so that it's just a single line.
    for (size_t band_index = 0; band_index < paths_.size(); ++band_index) {
        auto&       path = paths_.at(band_index);
        const auto& mags = magnitudes_.at(band_index);

        path.startNewSubPath(bounds_f_x, getYCoordinateFromMag(mags.at(0), bounds_f_y, bounds_f_bottom));

        for (int i = 1; i < bounds_i_width; ++i) {
            const auto x = bounds_f_x + static_cast< float >(i);
            const auto y = getYCoordinateFromMag(mags.at(i), bounds_f_y, bounds_f_bottom);

            path.lineTo(x, y);
        }
    }

    g.setColour(Theme::RESPONSE_CURVE_AVG);
    g.strokePath(paths_.at(paths_.size() - 1), juce::PathStrokeType(1.f));

    // Draw the nodes.
    for (size_t i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        if (peak_nodes_.at(i) == nullptr) {
            continue;
        }

        const auto& filter_id = static_cast< GraphicEq::FILTER_ID >(GraphicEq::PEAK_BAND_1 + i);

        const auto band_frequency = processor_ref_.getFilterFrequency(filter_id);
        const auto normalized_frequency = juce::mapFromLog10< float >(band_frequency, GraphicEq::MIN_HZ, GraphicEq::MAX_HZ);

        const auto x_raw = bounds_i_x + static_cast< int >(bounds_f_width * normalized_frequency);
        // Clamp the upper bounds for the x coordinate so that it can't go out of the vector bounds.
        const auto node_x = juce::jmin< int >(x_raw, bounds_i_width - NODE_RADIUS);
        const int node_y = getYCoordinateFromMag(magnitudes_.at(i).at(node_x - bounds_i_x), bounds_i_y, bounds_i_bottom);

        peak_nodes_.at(i)->setBounds(node_x - NODE_RADIUS, node_y - NODE_RADIUS, NODE_DIAMETER, NODE_DIAMETER);
        node_coordinates_.at(i).setXY(node_x, node_y);
    }

    // Vertical lines on both ends of the container to hide the edge of the response curve.
    g.setColour(Theme::MAIN_BACKGROUND);
    constexpr auto line_thickness = 3.f;

    g.drawLine(0.f, 0.f, 0.f, bounds_f.getBottom(), line_thickness);
    g.drawLine(bounds_f_width - 1.f, 0.f, bounds_f_width - 1.f, bounds_f.getBottom(), line_thickness);
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
    const auto x_coord = static_cast< int >(event.position.getX());
    const auto y_coord = static_cast< int >(event.position.getY());

    const auto& bounds        = getLocalBounds();
    const auto  bounds_width  = bounds.getWidth();
    const auto  bounds_height = bounds.getHeight();

    if (x_coord >= 0 && x_coord <= bounds_width && y_coord >= 0 && y_coord <= bounds_height) {
        const auto x_frequency = mapToLog10< float >(static_cast< float >(x_coord) / static_cast< float >(bounds_width),
                                                     GraphicEq::MIN_HZ,
                                                     GraphicEq::MAX_HZ);

        const auto y_decibels = juce::jmap< float >(static_cast< float >(y_coord),
                                                    0.f,
                                                    static_cast< float >(bounds_height),
                                                    GraphicEq::GRAPH_DB_MAX,
                                                    GraphicEq::GRAPH_DB_MIN);

        size_t closest_node      = 0;
        auto   shortest_distance = bounds_width;

        for (size_t i = 0; i < node_coordinates_.size(); ++i) {
            const auto distance = std::abs(node_coordinates_.at(i).getX() - x_coord);

            if (distance < shortest_distance) {
                closest_node      = i;
                shortest_distance = distance;
            }
        }

        if (peak_nodes_.at(closest_node) != nullptr && peak_nodes_.at(closest_node)->getToggleState()) {
            x_values_.at(closest_node) = x_frequency;
            y_values_.at(closest_node) = y_decibels;

            active_node_ = closest_node;
        }
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::parameterValueChanged(int parameter_index, float new_value)
{
    juce::ignoreUnused(parameter_index, new_value);

    parameters_changed_.set(true);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::parameterGestureChanged(int parameter_index, bool gesture_is_starting)
{
    juce::ignoreUnused(parameter_index, gesture_is_starting);

    // Do nothing. Only implemented because it's a pure virtual method of juce::AudioProcessorParameter::Listener.
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::timerCallback()
{
    if (parameters_changed_.compareAndSetBool(false, true)) {
        updateFilterChain();
        repaint();
    }
}

/*---------------------------------------------------------------------------
**
*/
size_t
GraphControlLayer::getActiveNodeIndex() const
{
    return active_node_;
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
GraphControlLayer::updateMagnitudes()
{
    const int  bounds_width = getLocalBounds().getWidth();
    const auto sample_rate  = processor_ref_.getSampleRate();

    auto& low_cut  = filter_chain_.get< GraphicEq::FILTER_ID::LOW_CUT >();
    auto& high_cut = filter_chain_.get< GraphicEq::FILTER_ID::HIGH_CUT >();

    std::array< juce::dsp::IIR::Filter< float >::CoefficientsPtr, GraphicEq::NUM_PEAK_BANDS > peak_coefficients;

    peak_coefficients[0] = filter_chain_.get< GraphicEq::FILTER_ID::PEAK_BAND_1 >().coefficients;
    peak_coefficients[1] = filter_chain_.get< GraphicEq::FILTER_ID::PEAK_BAND_2 >().coefficients;
    peak_coefficients[2] = filter_chain_.get< GraphicEq::FILTER_ID::PEAK_BAND_3 >().coefficients;
    peak_coefficients[3] = filter_chain_.get< GraphicEq::FILTER_ID::PEAK_BAND_4 >().coefficients;
    peak_coefficients[4] = filter_chain_.get< GraphicEq::FILTER_ID::PEAK_BAND_5 >().coefficients;

    const auto& low_shelf_coefficients  = filter_chain_.get< GraphicEq::FILTER_ID::LOW_SHELF >().coefficients;
    const auto& high_shelf_coefficients = filter_chain_.get< GraphicEq::FILTER_ID::HIGH_SHELF >().coefficients;

    for (auto i = 0; i < bounds_width; ++i) {
        std::array< double, GraphicEq::NUM_PEAK_BANDS > peak_mags { 0.0 };

        peak_mags.fill(1.0);

        double mag_low_cut    = 1.0;
        double mag_low_shelf  = 1.0;
        double mag_high_shelf = 1.0;
        double mag_high_cut   = 1.0;
        double mag_all        = 1.0;

        const auto freq = mapToLog10< double >(static_cast< double >(i) / bounds_width,
                                               GraphicEq::MIN_HZ,
                                               GraphicEq::MAX_HZ);

        if (processor_ref_.getFilterEnabled(GraphicEq::LOW_CUT)) {
            if (!low_cut.isBypassed< 0 >() && low_cut.get< 0 >().coefficients != nullptr) {
                const auto mag = low_cut.get< 0 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_low_cut *= mag;
                mag_all *= mag;
            }
            if (!low_cut.isBypassed< 1 >() && low_cut.get< 1 >().coefficients != nullptr) {
                const auto mag = low_cut.get< 1 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_low_cut *= mag;
                mag_all *= mag;
            }
            if (!low_cut.isBypassed< 2 >() && low_cut.get< 2 >().coefficients != nullptr) {
                const auto mag = low_cut.get< 2 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_low_cut *= mag;
                mag_all *= mag;
            }
            if (!low_cut.isBypassed< 3 >() && low_cut.get< 3 >().coefficients != nullptr) {
                const auto mag = low_cut.get< 3 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_low_cut *= mag;
                mag_all *= mag;
            }
        }

        if (processor_ref_.getFilterEnabled(GraphicEq::LOW_SHELF) && low_shelf_coefficients != nullptr) {
            const auto mag = low_shelf_coefficients->getMagnitudeForFrequency(freq, sample_rate);
            mag_low_shelf *= mag;
            mag_all *= mag;
        }

        for (int peak_band = 0; peak_band < GraphicEq::NUM_PEAK_BANDS; ++peak_band) {
            const auto& filter_id = static_cast< GraphicEq::FILTER_ID >(GraphicEq::PEAK_BAND_1 + peak_band);

            if (processor_ref_.getFilterEnabled(filter_id) && peak_coefficients.at(peak_band) != nullptr) {
                const auto mag = peak_coefficients.at(peak_band)->getMagnitudeForFrequency(freq, sample_rate);
                peak_mags.at(peak_band) *= mag;
                mag_all *= mag;
            }
        }

        if (processor_ref_.getFilterEnabled(GraphicEq::HIGH_SHELF) && high_shelf_coefficients != nullptr) {
            const auto mag = high_shelf_coefficients->getMagnitudeForFrequency(freq, sample_rate);
            mag_high_shelf *= mag;
            mag_all *= mag;
        }

        if (processor_ref_.getFilterEnabled(GraphicEq::HIGH_CUT)) {
            if (!high_cut.isBypassed< 0 >() && high_cut.get< 0 >().coefficients != nullptr) {
                const auto mag = high_cut.get< 0 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_high_cut *= mag;
                mag_all *= mag;
            }
            if (!high_cut.isBypassed< 1 >() && high_cut.get< 1 >().coefficients != nullptr) {
                const auto mag = high_cut.get< 1 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_high_cut *= mag;
                mag_all *= mag;
            }
            if (!high_cut.isBypassed< 2 >() && high_cut.get< 2 >().coefficients != nullptr) {
                const auto mag = high_cut.get< 2 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_high_cut *= mag;
                mag_all *= mag;
            }
            if (!high_cut.isBypassed< 3 >() && high_cut.get< 3 >().coefficients != nullptr) {
                const auto mag = high_cut.get< 3 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_high_cut *= mag;
                mag_all *= mag;
            }
        }

        magnitudes_.at(PATH_ID::LOW_CUT_PATH).at(i)   = juce::Decibels::gainToDecibels(mag_low_cut);
        magnitudes_.at(PATH_ID::LOW_SHELF_PATH).at(i) = juce::Decibels::gainToDecibels(mag_low_shelf);

        for (size_t peak_band = 0; peak_band < GraphicEq::NUM_PEAK_BANDS; ++peak_band) {
            magnitudes_.at(peak_band).at(i) = juce::Decibels::gainToDecibels(peak_mags.at(peak_band));
        }

        magnitudes_.at(PATH_ID::HIGH_SHELF_PATH).at(i) = juce::Decibels::gainToDecibels(mag_high_shelf);
        magnitudes_.at(PATH_ID::HIGH_CUT_PATH).at(i)   = juce::Decibels::gainToDecibels(mag_high_cut);

        magnitudes_.at(PATH_ID::ALL).at(i) = juce::Decibels::gainToDecibels(mag_all);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::addListeners()
{
    if (!allControlPtrsValid()) {
        return;
    }

    auto& apvts = processor_ref_.getApvts();

    const auto& low_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_CUT);
    apvts.getParameter(low_cut_parameters.enabled_)->addListener(this);
    apvts.getParameter(low_cut_parameters.frequency_)->addListener(this);
    apvts.getParameter(low_cut_parameters.slope_)->addListener(this);

    const auto& high_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_CUT);
    apvts.getParameter(high_cut_parameters.enabled_)->addListener(this);
    apvts.getParameter(high_cut_parameters.frequency_)->addListener(this);
    apvts.getParameter(high_cut_parameters.slope_)->addListener(this);

    const auto& low_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_SHELF);
    apvts.getParameter(low_shelf_parameters.enabled_)->addListener(this);
    apvts.getParameter(low_shelf_parameters.frequency_)->addListener(this);
    apvts.getParameter(low_shelf_parameters.gain_)->addListener(this);

    const auto& high_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_SHELF);
    apvts.getParameter(high_shelf_parameters.enabled_)->addListener(this);
    apvts.getParameter(high_shelf_parameters.frequency_)->addListener(this);
    apvts.getParameter(high_shelf_parameters.gain_)->addListener(this);

    for (int i = GraphicEq::PEAK_BAND_1; i <= GraphicEq::PEAK_BAND_5; ++i) {
        const auto& peak_band_parameters = GraphicEq::PARAMETER_MAP.at(static_cast< GraphicEq::FILTER_ID >(i));

        apvts.getParameter(peak_band_parameters.enabled_)->addListener(this);
        apvts.getParameter(peak_band_parameters.frequency_)->addListener(this);
        apvts.getParameter(peak_band_parameters.gain_)->addListener(this);
        apvts.getParameter(peak_band_parameters.q_)->addListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::removeListeners()
{
    if (!allControlPtrsValid()) {
        return;
    }

    auto& apvts = processor_ref_.getApvts();

    const auto& low_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_CUT);
    apvts.getParameter(low_cut_parameters.enabled_)->removeListener(this);
    apvts.getParameter(low_cut_parameters.frequency_)->removeListener(this);
    apvts.getParameter(low_cut_parameters.slope_)->removeListener(this);

    const auto& high_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_CUT);
    apvts.getParameter(high_cut_parameters.enabled_)->removeListener(this);
    apvts.getParameter(high_cut_parameters.frequency_)->removeListener(this);
    apvts.getParameter(high_cut_parameters.slope_)->removeListener(this);

    const auto& low_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_SHELF);
    apvts.getParameter(low_shelf_parameters.enabled_)->removeListener(this);
    apvts.getParameter(low_shelf_parameters.frequency_)->removeListener(this);
    apvts.getParameter(low_shelf_parameters.gain_)->removeListener(this);

    const auto& high_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_SHELF);
    apvts.getParameter(high_shelf_parameters.enabled_)->removeListener(this);
    apvts.getParameter(high_shelf_parameters.frequency_)->removeListener(this);
    apvts.getParameter(high_shelf_parameters.gain_)->removeListener(this);

    for (int i = GraphicEq::PEAK_BAND_1; i <= GraphicEq::PEAK_BAND_5; ++i) {
        const auto& peak_band_parameters = GraphicEq::PARAMETER_MAP.at(static_cast< GraphicEq::FILTER_ID >(i));

        apvts.getParameter(peak_band_parameters.enabled_)->removeListener(this);
        apvts.getParameter(peak_band_parameters.frequency_)->removeListener(this);
        apvts.getParameter(peak_band_parameters.gain_)->removeListener(this);
        apvts.getParameter(peak_band_parameters.q_)->removeListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphControlLayer::allControlPtrsValid() const
{
    const auto& apvts = processor_ref_.getApvts();
    bool        ok    = true;

    const auto& low_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_CUT);
    ok &= apvts.getParameter(low_cut_parameters.enabled_) != nullptr;
    ok &= apvts.getParameter(low_cut_parameters.frequency_) != nullptr;
    ok &= apvts.getParameter(low_cut_parameters.slope_) != nullptr;

    const auto& high_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_CUT);
    ok &= apvts.getParameter(high_cut_parameters.enabled_) != nullptr;
    ok &= apvts.getParameter(high_cut_parameters.frequency_) != nullptr;
    ok &= apvts.getParameter(high_cut_parameters.slope_) != nullptr;

    const auto& low_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_SHELF);
    ok &= apvts.getParameter(low_shelf_parameters.enabled_) != nullptr;
    ok &= apvts.getParameter(low_shelf_parameters.frequency_) != nullptr;
    ok &= apvts.getParameter(low_shelf_parameters.gain_) != nullptr;

    const auto& high_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_SHELF);
    ok &= apvts.getParameter(high_shelf_parameters.enabled_) != nullptr;
    ok &= apvts.getParameter(high_shelf_parameters.frequency_) != nullptr;
    ok &= apvts.getParameter(high_shelf_parameters.gain_) != nullptr;

    for (int i = GraphicEq::PEAK_BAND_1; i <= GraphicEq::PEAK_BAND_5; ++i) {
        const auto& peak_band_parameters = GraphicEq::PARAMETER_MAP.at(static_cast< GraphicEq::FILTER_ID >(i));

        ok &= apvts.getParameter(peak_band_parameters.enabled_) != nullptr;
        ok &= apvts.getParameter(peak_band_parameters.frequency_) != nullptr;
        ok &= apvts.getParameter(peak_band_parameters.gain_) != nullptr;
        ok &= apvts.getParameter(peak_band_parameters.q_) != nullptr;
    }

    return ok;
}

/*---------------------------------------------------------------------------
** End of File
*/
