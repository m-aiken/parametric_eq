#pragma once

#include <JuceHeader.h>

#include "AnalyzerContainer.h"
#include "CustomLookAndFeel.h"
#include "EqControls.h"
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

    static void drawDbLabels(juce::Graphics& g, const juce::Rectangle< int >& label_bounds);
    static void drawFrequencyLabels(juce::Graphics& g, const juce::Rectangle< int >& label_bounds);

private:
    GraphicEqProcessor& processor_ref_;

    AnalyzerContainer analyzer_container_;
    EqControls        eq_controls_;

    size_t active_node_;

    CustomLookAndFeel lnf_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEqEditor)
};
