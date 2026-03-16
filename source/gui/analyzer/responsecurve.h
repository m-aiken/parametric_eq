#pragma once

#include <JuceHeader.h>

#include "FilterUtils.h"
#include "GraphicEqGlobals.h"
#include "PeakBand.h"

struct Node : juce::ToggleButton {
    Node(juce::AudioProcessorValueTreeState& _apvts, int _bandNum);
    void paint(juce::Graphics& g) override;

private:
    std::unique_ptr< juce::AudioProcessorValueTreeState::ButtonAttachment > paramAttachment;
};

/*---------------------------------------------------------------------------
**
*/
struct ResponseCurve
    : juce::Component
    , juce::AudioProcessorParameter::Listener
    , juce::Timer {
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

    std::array< juce::Point< int >, GraphicEqGlobals::getNumPeakBands() >& getNodeCoordinates()
    {
        return nodeCoordinates;
    }

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

    std::array< PeakBand, GraphicEqGlobals::getNumPeakBands() >                peakBands;
    std::array< std::unique_ptr< Node >, GraphicEqGlobals::getNumPeakBands() > peakNodes;
    std::array< juce::Point< int >, GraphicEqGlobals::getNumPeakBands() >      nodeCoordinates;

    std::array< juce::Value, GraphicEqGlobals::getNumPeakBands() > xValues;
    std::array< juce::Value, GraphicEqGlobals::getNumPeakBands() > yValues;

    size_t activeNode;
};
