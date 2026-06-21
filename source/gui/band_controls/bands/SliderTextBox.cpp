#include "SliderTextBox.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
SliderTextBox::SliderTextBox(const juce::String& component_name, const juce::String& label_text)
    : juce::Label(component_name, label_text)
{
    setFont(parametric_eq::theme::getFont());
    setJustificationType(juce::Justification::centred);
    setInterceptsMouseClicks(false, false);

    setColour(textColourId, parametric_eq::theme::TEXT);
    setColour(textWhenEditingColourId, parametric_eq::theme::TEXT);
    setColour(outlineWhenEditingColourId, juce::Colours::white.withAlpha(0.f));
    setColour(juce::TextEditor::ColourIds::highlightedTextColourId, parametric_eq::theme::TEXT);
    setColour(juce::TextEditor::ColourIds::highlightColourId, parametric_eq::theme::TEXT.withAlpha(0.25f));
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
