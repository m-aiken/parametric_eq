#pragma once

#include <JuceHeader.h>

#include "FilterFactory.h"
#include "GraphNode.h"
#include "ParametricEqGlobals.h"

class GraphControlLayer final
    : public juce::Component
    , public juce::AudioProcessorValueTreeState::Listener
    , public juce::Timer
{
public:
    explicit GraphControlLayer(ParametricEqProcessor& processor_ref);
    ~GraphControlLayer() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void parameterChanged(const juce::String& parameter_id, float new_value) override;
    void timerCallback() override;

    void setActiveBand(const parametric_eq::FILTER_ID& active_band) const;

private:
    void updateFilterChain();

    void          calculateFilterMagnitudeResponse(const parametric_eq::FILTER_ID& filter_id);
    void          calculateTotalMagnitudeResponse();
    static double getHzFromPx(const int px, const int max_px);
    static float  getPathYCoordinate(const double magnitude, const float bounds_top, const float bounds_bottom);
    static int    getNodeYCoordinate(const double magnitude, const float bounds_top, const float bounds_bottom);
    int           getNodeXCoordinate(const parametric_eq::FILTER_ID& filter_id, const float bounds_width) const;

    void resetUpdateFlags(const bool should_update = false);
    void initialiseCoefficientsRefs();

    ParametricEqProcessor& processor_ref_;
    juce::Atomic< bool >   filter_parameters_changed_;

    parametric_eq::FilterChain filter_chain_;

    std::array< std::unique_ptr< GraphNode >, parametric_eq::NUM_FILTERS > nodes_;
    std::array< juce::Value, parametric_eq::NUM_FILTERS >                  x_values_;
    std::array< juce::Value, parametric_eq::NUM_FILTERS >                  y_values_;
    std::array< juce::Path, parametric_eq::NUM_FILTERS + 1 >               paths_;
    std::array< std::vector< double >, parametric_eq::NUM_FILTERS + 1 >    magnitudes_;

    size_t                  latched_node_index_;
    static constexpr size_t LATCHED_NODE_INDEX_UNSET = 0xFFFF;
    static constexpr int    DRAG_PROXIMITY_LIMIT     = 36;

    std::array< juce::Atomic< bool >, parametric_eq::NUM_FILTERS > update_flags_;

    typedef juce::dsp::IIR::Filter< float >::CoefficientsPtr               FilterCoefficients;
    std::array< FilterCoefficients, parametric_eq::NUM_CUT_SLOPE_OPTIONS > lcut_coefficients_;
    std::array< FilterCoefficients, parametric_eq::NUM_CUT_SLOPE_OPTIONS > hcut_coefficients_;
    std::array< FilterCoefficients, parametric_eq::NUM_FILTERS >           band_coefficients_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphControlLayer)
};
