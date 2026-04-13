#pragma once

#include <JuceHeader.h>

#include "EqControls.h"
#include "Graph.h"
#include "ParametricEqProcessor.h"

class ParametricEqEditor final : public juce::AudioProcessorEditor
{
public:
    explicit ParametricEqEditor(ParametricEqProcessor& p);
    ~ParametricEqEditor() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ParametricEqProcessor& processor_ref_;

    Graph      graph_;
    EqControls eq_controls_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametricEqEditor)
};
