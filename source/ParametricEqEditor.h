#pragma once

#include <JuceHeader.h>

#include "EqControls.h"
#include "Graph.h"
#include "MenuBar.h"

class ParametricEqEditor final
    : public juce::AudioProcessorEditor
    , public juce::AudioProcessorValueTreeState::Listener
    , public juce::Timer
{
public:
    explicit ParametricEqEditor(ParametricEqProcessor& p);
    ~ParametricEqEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void parameterChanged(const juce::String& parameter_id, float new_value) override;
    void timerCallback() override;

private:
    ParametricEqProcessor& processor_ref_;

    MenuBar    menu_bar_;
    Graph      graph_;
    EqControls eq_controls_;

    ParametricEq::FILTER_ID queued_active_band_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametricEqEditor)
};
