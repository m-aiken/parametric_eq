#pragma once

#include <JuceHeader.h>

#include "CustomLookAndFeel.h"
#include "EqControls.h"
#include "Graph.h"
#include "GraphicEqProcessor.h"

class GraphicEqEditor final : public juce::AudioProcessorEditor
{
public:
    explicit GraphicEqEditor(GraphicEqProcessor& p);
    ~GraphicEqEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    GraphicEqProcessor& processor_ref_;

    Graph      graph_;
    EqControls eq_controls_;

    CustomLookAndFeel lnf_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEqEditor)
};
