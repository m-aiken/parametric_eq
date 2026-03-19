#include "AnalyzerControls.h"
#include "AnalyzerProperties.h"
#include "ColourPalette.h"
#include "GraphicEqGlobals.h"

/*---------------------------------------------------------------------------
**
*/
AnalyzerControls::AnalyzerControls(juce::AudioProcessorValueTreeState& apvts)
    : fftOrderSlider(apvts.getParameter(
          AnalyzerProperties::getAnalyzerParams().at(AnalyzerProperties::ParamNames::Analyzer_Points)))
    , fftOrderAttachment(apvts,
                         AnalyzerProperties::getAnalyzerParams().at(AnalyzerProperties::ParamNames::Analyzer_Points),
                         fftOrderSlider)
{
    addAndMakeVisible(fftOrderSlider);
}

/*---------------------------------------------------------------------------
**
*/
void
AnalyzerControls::paint(juce::Graphics& g)
{
    auto bounds       = getLocalBounds();
    auto sliderWidth  = bounds.getWidth() * 0.4;
    auto sliderHeight = GraphicEq::FONT.getHeight();

    g.setFont(GraphicEq::FONT);
    g.setColour(ColourPalette::Text);

    g.drawFittedText("Resolution (low <--> high)", 0, 0, sliderWidth, sliderHeight, juce::Justification::centred, 1);
}

/*---------------------------------------------------------------------------
**
*/
void
AnalyzerControls::resized()
{
    const auto& bounds       = getLocalBounds();
    const auto& sliderWidth  = bounds.getWidth() * 0.4;
    const auto& sliderHeight = GraphicEq::FONT.getHeight();
    const auto& padding      = sliderHeight * 0.2;

    fftOrderSlider.setBounds(0, sliderHeight + padding, sliderWidth, sliderHeight);
}

/*---------------------------------------------------------------------------
** End of File
*/
