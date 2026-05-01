#pragma once

#include <JuceHeader.h>

class SliderTextBox final : public juce::Label
{
public:
    explicit SliderTextBox(const juce::String& component_name = juce::String(),
                           const juce::String& label_text     = juce::String());

protected:
    void editorShown(juce::TextEditor* editor) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderTextBox)
};
