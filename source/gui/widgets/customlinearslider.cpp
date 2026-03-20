#include "CustomLinearSlider.h"
#include "GraphicEqGlobals.h"

/*---------------------------------------------------------------------------
**
*/
CustomLinearSlider::CustomLinearSlider(juce::RangedAudioParameter* rap)
    : juce::Slider(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextEntryBoxPosition::NoTextBox)
    , param(rap)
{
}

/*---------------------------------------------------------------------------
**
*/
void
CustomLinearSlider::paint(juce::Graphics& g)
{
    if (param == nullptr) {
        return;
    }

    auto bounds = getLocalBounds();

    auto paramNormalisableRange = param->getNormalisableRange();
    auto scaledValue            = juce::jmap< float >(param->convertFrom0to1(param->getValue()),
                                           paramNormalisableRange.start,
                                           paramNormalisableRange.end,
                                           bounds.getX(),
                                           bounds.getRight());

    getLookAndFeel().drawLinearSlider(g,                                            // graphics
                                      bounds.getX(),                                // x
                                      bounds.getY(),                                // y
                                      bounds.getWidth(),                            // width
                                      bounds.getHeight(),                           // height
                                      scaledValue,                                  // slider position
                                      bounds.getX(),                                // min slider position
                                      bounds.getRight(),                            // max slider position
                                      juce::Slider::SliderStyle::LinearHorizontal,  // slider style
                                      *this);                                       // slider
}

/*---------------------------------------------------------------------------
**
*/
void
CustomLinearSlider::setEnablement(const bool enable)
{
    setEnabled(enable);
    setAlpha(enable ? GraphicEq::WIDGET_ALPHA_FULL : GraphicEq::WIDGET_ALPHA_DIM);
}

/*---------------------------------------------------------------------------
** End of File
*/
