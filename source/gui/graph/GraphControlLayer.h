#pragma once

#include <JuceHeader.h>

#include "FilterFactory.h"
#include "GraphNode.h"
#include "GraphicEqGlobals.h"

class GraphControlLayer final
    : public juce::Component
    , public juce::AudioProcessorParameter::Listener
    , public juce::Timer
{
public:
    explicit GraphControlLayer(GraphicEqProcessor& processor_ref);
    ~GraphControlLayer() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void parameterValueChanged(int parameter_index, float new_value) override;
    void parameterGestureChanged(int parameter_index, bool gesture_is_starting) override;
    void timerCallback() override;

    size_t getActiveNodeIndex() const;

private:
    void updateMonoChain();
    void updateMagnitudes();

    template < typename T >
    static T getYCoordinateFromMag(const double magnitude, const T bounds_top, const T bounds_bottom)
    {
        const auto mapped = juce::jmap< float >(static_cast< float >(magnitude),
                                                GraphicEq::MIN_DB,
                                                GraphicEq::MAX_DB,
                                                static_cast< float >(bounds_bottom),
                                                static_cast< float >(bounds_top));

        return static_cast< T >(mapped);
    }

    void addListeners();
    void removeListeners();
    bool allControlPtrsValid() const;

    static constexpr int NODE_DIAMETER = 6;
    static constexpr int NODE_RADIUS   = 3;

    GraphicEqProcessor&  processor_ref_;
    juce::Atomic< bool > parameters_changed_;
    size_t               active_node_;

    FilterFactory::MonoChain mono_chain_;

    std::array< std::unique_ptr< GraphNode >, GraphicEq::NUM_PEAK_BANDS > peak_nodes_;
    std::array< juce::Point< int >, GraphicEq::NUM_PEAK_BANDS >           node_coordinates_;
    std::array< juce::Value, GraphicEq::NUM_PEAK_BANDS >                  x_values_;
    std::array< juce::Value, GraphicEq::NUM_PEAK_BANDS >                  y_values_;

    enum PATH_ID {
        P0,
        P1,
        P2,
        P3,
        P4,
        P5,
        LC,
        HC,
        ALL,
        NUM_PATHS,
    };

    std::array< juce::Path, NUM_PATHS >            paths_;
    std::array< std::vector< double >, NUM_PATHS > magnitudes_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphControlLayer)
};
