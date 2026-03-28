#pragma once

#include <JuceHeader.h>

#include "CustomLookAndFeel.h"
#include "EqControls.h"
#include "Graph.h"
#include "GraphicEqProcessor.h"

class GraphicEqEditor final
    : public juce::AudioProcessorEditor
    , public juce::Timer
{
public:
    explicit GraphicEqEditor(GraphicEqProcessor& p);
    ~GraphicEqEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    GraphicEqProcessor& processor_ref_;

    Graph      graph_;
    EqControls eq_controls_;

    size_t active_node_;

    CustomLookAndFeel lnf_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEqEditor)
};
