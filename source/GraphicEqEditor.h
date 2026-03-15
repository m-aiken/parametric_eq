#pragma once

#include <JuceHeader.h>

#include "AnalyzerContainer.h"
#include "AnalyzerControls.h"
#include "CustomLookAndFeel.h"
#include "EqControlsContainer.h"
#include "GraphicEqProcessor.h"
#include "ThemeButton.h"

//==============================================================================
class GraphicEqEditor : public juce::AudioProcessorEditor, juce::Timer
{
public:
    explicit GraphicEqEditor(GraphicEqProcessor&);
    ~GraphicEqEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void drawDbLabels(juce::Graphics& g, juce::Rectangle<int>& labelBounds);
    void drawFrequencyLabels(juce::Graphics& g, juce::Rectangle<int>& labelBounds);

    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GraphicEqProcessor& processorRef;

    std::unique_ptr<AnalyzerControls>    analyzerControls;
    std::unique_ptr<ThemeButton>         themeButton;
    std::unique_ptr<AnalyzerContainer>   analyzerContainer;
    std::unique_ptr<EqControlsContainer> eqControls;

    CustomLookAndFeel lnf;

    size_t activeNode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEqEditor)
};
