#include "SliderTextBox.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
SliderTextBox::SliderTextBox(const juce::String& component_name, const juce::String& label_text)
    : juce::Label(component_name, label_text)
{
    setFont(ParametricEq::Theme::getFont());
    setJustificationType(juce::Justification::centred);
    setInterceptsMouseClicks(false, false);

    setColour(textColourId, ParametricEq::Theme::TEXT);
    setColour(textWhenEditingColourId, ParametricEq::Theme::TEXT);
    setColour(outlineWhenEditingColourId, juce::Colours::white.withAlpha(0.f));
    setColour(juce::TextEditor::ColourIds::highlightedTextColourId, ParametricEq::Theme::TEXT);
    setColour(juce::TextEditor::ColourIds::highlightColourId, ParametricEq::Theme::TEXT.withAlpha(0.25f));
}

/*---------------------------------------------------------------------------
**
*/
void
SliderTextBox::editorShown(juce::TextEditor* editor)
{
    if (editor != nullptr) {
        editor->setJustification(juce::Justification::centred);
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
