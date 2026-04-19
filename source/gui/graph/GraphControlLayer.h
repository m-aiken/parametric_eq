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

private:
    void updateFilterChain();
    void calculateFilterMagnitudeResponse(const ParametricEq::FILTER_ID& filter_id);
    void calculateTotalMagnitudeResponse();
    void resetUpdateFlags();

    template < typename T >
    static T getYCoordinateFromMag(const double magnitude, const T bounds_top, const T bounds_bottom)
    {
        const auto mapped = juce::jmap< float >(static_cast< float >(magnitude),
                                                ParametricEq::MIN_DB,
                                                ParametricEq::MAX_DB,
                                                static_cast< float >(bounds_bottom),
                                                static_cast< float >(bounds_top));

        return static_cast< T >(mapped);
    }

    void addListeners();
    void removeListeners();
    bool allControlPtrsValid() const;
    void initialiseCoefficientsRefs();

    ParametricEqProcessor& processor_ref_;
    juce::Atomic< bool >   parameters_changed_;

    ParametricEq::FilterChain filter_chain_;

    std::array< std::unique_ptr< GraphNode >, ParametricEq::NUM_FILTERS > nodes_;
    std::array< juce::Value, ParametricEq::NUM_FILTERS >                  x_values_;
    std::array< juce::Value, ParametricEq::NUM_FILTERS >                  y_values_;
    std::array< juce::Path, ParametricEq::NUM_FILTERS + 1 >               paths_;
    std::array< std::vector< double >, ParametricEq::NUM_FILTERS + 1 >    magnitudes_;

    size_t                  latched_node_index_;
    static constexpr size_t LATCHED_NODE_INDEX_UNSET = 0xFFFF;

    std::array< juce::Atomic< bool >, ParametricEq::NUM_FILTERS > update_flags_;

    typedef juce::dsp::IIR::Filter< float >::CoefficientsPtr              FilterCoefficients;
    std::array< FilterCoefficients, ParametricEq::NUM_CUT_SLOPE_OPTIONS > low_cut_coefficients_;
    std::array< FilterCoefficients, ParametricEq::NUM_CUT_SLOPE_OPTIONS > high_cut_coefficients_;
    std::array< FilterCoefficients, ParametricEq::NUM_FILTERS >           band_coefficients_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphControlLayer)
};
