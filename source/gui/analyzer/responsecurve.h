#pragma once

#include <JuceHeader.h>

#include "FilterUtils.h"
#include "GraphicEqGlobals.h"
#include "PeakBand.h"

class Node final : public juce::ToggleButton
{
public:
    Node(juce::AudioProcessorValueTreeState& _apvts, int _bandNum);
    void paint(juce::Graphics& g) override;

private:
    juce::AudioProcessorValueTreeState::ButtonAttachment paramAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Node)
};

/*---------------------------------------------------------------------------
**
*/
class ResponseCurve final
    : public juce::Component
    , public juce::AudioProcessorParameter::Listener
    , public juce::Timer
{
public:
    ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate);
    ~ResponseCurve();

    void paint(juce::Graphics& g) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    void                                   updateMonoChain();
    std::array< std::vector< double >, 9 > getMagnitudes(int boundsWidth);

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override { }

    void timerCallback() override;

    void addListeners();
    void removeListeners();
    bool allControlPtrsValid();

    std::array< juce::Point< int >, GraphicEq::NUM_PEAK_BANDS >& getNodeCoordinates() { return nodeCoordinates; }

    size_t getActiveNodeIndex() { return activeNode; }

private:
    double sampleRate;
    int    nodeDiameter { 6 };
    int    nodeRadius { 3 };

    juce::Atomic< bool > parametersChanged { false };

    juce::AudioProcessorValueTreeState& apvts;
    FilterUtils::MonoChain              monoChain;

    juce::AudioParameterBool*   lowCutEnabledParam { nullptr };
    juce::AudioParameterFloat*  lowCutFreqParam { nullptr };
    juce::AudioParameterChoice* lowCutSlopeParam { nullptr };
    juce::AudioParameterBool*   highCutEnabledParam { nullptr };
    juce::AudioParameterFloat*  highCutFreqParam { nullptr };
    juce::AudioParameterChoice* highCutSlopeParam { nullptr };

    std::array< PeakBand, GraphicEq::NUM_PEAK_BANDS >                peakBands;
    std::array< std::unique_ptr< Node >, GraphicEq::NUM_PEAK_BANDS > peakNodes;
    std::array< juce::Point< int >, GraphicEq::NUM_PEAK_BANDS >      nodeCoordinates;
    std::array< juce::Value, GraphicEq::NUM_PEAK_BANDS >             xValues;
    std::array< juce::Value, GraphicEq::NUM_PEAK_BANDS >             yValues;

    size_t activeNode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponseCurve)
};
