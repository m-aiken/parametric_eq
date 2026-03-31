#include "EqProperties.h"
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
        peak_nodes_.at(i) = std::make_unique< GraphNode >(apvts, i);
    }

    using namespace EqProperties;

    for (size_t i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        addAndMakeVisible(*peak_nodes_.at(i));

        node_coordinates_.at(i).setXY(0, 0);

        x_values_.at(i).referTo(apvts.getParameterAsValue(getPeakControlParamName(PeakControl::FREQUENCY, i)));
        y_values_.at(i).referTo(apvts.getParameterAsValue(getPeakControlParamName(PeakControl::GAIN, i)));
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

        const auto band_frequency = processor_ref_.getBandFrequency(i);
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
                                                    GraphicEq::MAX_DB,
                                                    GraphicEq::MIN_DB);

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
    const FilterFactory& filter_factory = processor_ref_.getFilterFactory();

    filter_factory.updateBandEnablements(filter_chain_);

    if (processor_ref_.getLowCutEnabled()) {
        filter_factory.updateCutCoefficients(filter_chain_, FilterFactory::ChainPositions::LowCut);
    }

    if (processor_ref_.getHighCutEnabled()) {
        filter_factory.updateCutCoefficients(filter_chain_, FilterFactory::ChainPositions::HighCut);
    }

    for (int i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        if (processor_ref_.getBandEnabled(i)) {
            const auto chain_pos = static_cast< FilterFactory::ChainPositions >(i + 1);

            filter_factory.updatePeakCoefficients(filter_chain_, chain_pos, i);
        }
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::updateMagnitudes()
{
    const int  bounds_width = getLocalBounds().getWidth();
    const auto sample_rate  = processor_ref_.getSampleRate();

    std::array< juce::dsp::IIR::Filter< float >::CoefficientsPtr, GraphicEq::NUM_PEAK_BANDS > peak_coefficients;

    peak_coefficients[0] = filter_chain_.get< FilterFactory::ChainPositions::Peak_0 >().coefficients;
    peak_coefficients[1] = filter_chain_.get< FilterFactory::ChainPositions::Peak_1 >().coefficients;
    peak_coefficients[2] = filter_chain_.get< FilterFactory::ChainPositions::Peak_2 >().coefficients;
    peak_coefficients[3] = filter_chain_.get< FilterFactory::ChainPositions::Peak_3 >().coefficients;
    peak_coefficients[4] = filter_chain_.get< FilterFactory::ChainPositions::Peak_4 >().coefficients;
    peak_coefficients[5] = filter_chain_.get< FilterFactory::ChainPositions::Peak_5 >().coefficients;

    auto& low_cut  = filter_chain_.get< FilterFactory::ChainPositions::LowCut >();
    auto& high_cut = filter_chain_.get< FilterFactory::ChainPositions::HighCut >();

    for (auto i = 0; i < bounds_width; ++i) {
        std::array< double, GraphicEq::NUM_PEAK_BANDS > peak_mags { 0.0 };

        peak_mags.fill(1.0);

        double mag_lc  = 1.0;
        double mag_hc  = 1.0;
        double mag_all = 1.0;

        const auto freq = mapToLog10< double >(static_cast< double >(i) / bounds_width,
                                               GraphicEq::MIN_HZ,
                                               GraphicEq::MAX_HZ);

        for (size_t peak_band = 0; peak_band < GraphicEq::NUM_PEAK_BANDS; ++peak_band) {
            if (processor_ref_.getBandEnabled(peak_band) && peak_coefficients.at(peak_band) != nullptr) {
                peak_mags.at(peak_band) *= peak_coefficients.at(peak_band)->getMagnitudeForFrequency(freq, sample_rate);
                mag_all *= peak_coefficients.at(peak_band)->getMagnitudeForFrequency(freq, sample_rate);
            }
        }

        if (processor_ref_.getLowCutEnabled()) {
            if (!low_cut.isBypassed< 0 >() && low_cut.get< 0 >().coefficients != nullptr) {
                const auto mag = low_cut.get< 0 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_lc *= mag;
                mag_all *= mag;
            }
            if (!low_cut.isBypassed< 1 >() && low_cut.get< 1 >().coefficients != nullptr) {
                const auto mag = low_cut.get< 1 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_lc *= mag;
                mag_all *= mag;
            }
            if (!low_cut.isBypassed< 2 >() && low_cut.get< 2 >().coefficients != nullptr) {
                const auto mag = low_cut.get< 2 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_lc *= mag;
                mag_all *= mag;
            }
            if (!low_cut.isBypassed< 3 >() && low_cut.get< 3 >().coefficients != nullptr) {
                const auto mag = low_cut.get< 3 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_lc *= mag;
                mag_all *= mag;
            }
        }

        if (processor_ref_.getHighCutEnabled()) {
            if (!high_cut.isBypassed< 0 >() && high_cut.get< 0 >().coefficients != nullptr) {
                const auto mag = high_cut.get< 0 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_hc *= mag;
                mag_all *= mag;
            }
            if (!high_cut.isBypassed< 1 >() && high_cut.get< 1 >().coefficients != nullptr) {
                const auto mag = high_cut.get< 1 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_hc *= mag;
                mag_all *= mag;
            }
            if (!high_cut.isBypassed< 2 >() && high_cut.get< 2 >().coefficients != nullptr) {
                const auto mag = high_cut.get< 2 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_hc *= mag;
                mag_all *= mag;
            }
            if (!high_cut.isBypassed< 3 >() && high_cut.get< 3 >().coefficients != nullptr) {
                const auto mag = high_cut.get< 3 >().coefficients->getMagnitudeForFrequency(freq, sample_rate);
                mag_hc *= mag;
                mag_all *= mag;
            }
        }

        for (size_t peak_band = 0; peak_band < GraphicEq::NUM_PEAK_BANDS; ++peak_band) {
            magnitudes_.at(peak_band).at(i) = juce::Decibels::gainToDecibels(peak_mags.at(peak_band));
        }

        magnitudes_.at(PATH_ID::LC).at(i)  = juce::Decibels::gainToDecibels(mag_lc);
        magnitudes_.at(PATH_ID::HC).at(i)  = juce::Decibels::gainToDecibels(mag_hc);
        magnitudes_.at(PATH_ID::ALL).at(i) = juce::Decibels::gainToDecibels(mag_all);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::addListeners()
{
    using namespace GraphicEq;
    using namespace EqProperties;

    if (!allControlPtrsValid()) {
        return;
    }

    auto& apvts = processor_ref_.getApvts();

    apvts.getParameter(LOW_CUT_FREQ.id_)->addListener(this);
    apvts.getParameter(LOW_CUT_SLOPE.id_)->addListener(this);
    apvts.getParameter(LOW_CUT_ENABLED.id_)->addListener(this);

    apvts.getParameter(HIGH_CUT_FREQ.id_)->addListener(this);
    apvts.getParameter(HIGH_CUT_SLOPE.id_)->addListener(this);
    apvts.getParameter(HIGH_CUT_ENABLED.id_)->addListener(this);

    for (int i = 0; i < NUM_PEAK_BANDS; ++i) {
        apvts.getParameter(getPeakControlParamName(PeakControl::FREQUENCY, i))->addListener(this);
        apvts.getParameter(getPeakControlParamName(PeakControl::GAIN, i))->addListener(this);
        apvts.getParameter(getPeakControlParamName(PeakControl::QUALITY, i))->addListener(this);
        apvts.getParameter(getPeakControlParamName(PeakControl::ENABLED, i))->addListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphControlLayer::removeListeners()
{
    using namespace GraphicEq;
    using namespace EqProperties;

    if (!allControlPtrsValid()) {
        return;
    }

    auto& apvts = processor_ref_.getApvts();

    apvts.getParameter(LOW_CUT_FREQ.id_)->removeListener(this);
    apvts.getParameter(LOW_CUT_SLOPE.id_)->removeListener(this);
    apvts.getParameter(LOW_CUT_ENABLED.id_)->removeListener(this);

    apvts.getParameter(HIGH_CUT_FREQ.id_)->removeListener(this);
    apvts.getParameter(HIGH_CUT_SLOPE.id_)->removeListener(this);
    apvts.getParameter(HIGH_CUT_ENABLED.id_)->removeListener(this);

    for (int i = 0; i < NUM_PEAK_BANDS; ++i) {
        apvts.getParameter(getPeakControlParamName(PeakControl::FREQUENCY, i))->removeListener(this);
        apvts.getParameter(getPeakControlParamName(PeakControl::GAIN, i))->removeListener(this);
        apvts.getParameter(getPeakControlParamName(PeakControl::QUALITY, i))->removeListener(this);
        apvts.getParameter(getPeakControlParamName(PeakControl::ENABLED, i))->removeListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphControlLayer::allControlPtrsValid() const
{
    using namespace GraphicEq;
    using namespace EqProperties;

    const auto& apvts = processor_ref_.getApvts();
    bool        ok    = true;

    ok &= apvts.getParameter(LOW_CUT_FREQ.id_) != nullptr;
    ok &= apvts.getParameter(LOW_CUT_SLOPE.id_) != nullptr;
    ok &= apvts.getParameter(LOW_CUT_ENABLED.id_) != nullptr;

    ok &= apvts.getParameter(HIGH_CUT_FREQ.id_) != nullptr;
    ok &= apvts.getParameter(HIGH_CUT_SLOPE.id_) != nullptr;
    ok &= apvts.getParameter(HIGH_CUT_ENABLED.id_) != nullptr;

    for (int i = 0; i < NUM_PEAK_BANDS; ++i) {
        ok &= apvts.getParameter(getPeakControlParamName(PeakControl::FREQUENCY, i)) != nullptr;
        ok &= apvts.getParameter(getPeakControlParamName(PeakControl::GAIN, i)) != nullptr;
        ok &= apvts.getParameter(getPeakControlParamName(PeakControl::QUALITY, i)) != nullptr;
        ok &= apvts.getParameter(getPeakControlParamName(PeakControl::ENABLED, i)) != nullptr;
    }

    return ok;
}

/*---------------------------------------------------------------------------
** End of File
*/
