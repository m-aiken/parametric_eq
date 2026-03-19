#include "ColourPalette.h"
#include "CutControl.h"
#include "EqProperties.h"

/*---------------------------------------------------------------------------
**
*/
CutControl::CutControl(juce::AudioProcessorValueTreeState& apvts, const FilterUtils::ChainPositions& _chainPosition)
    : chainPosition(_chainPosition)
{
    const bool  low_cut    = chainPosition == FilterUtils::ChainPositions::LowCut;
    const auto& freq_id    = low_cut ? GraphicEq::LOW_CUT_FREQ.id_ : GraphicEq::HIGH_CUT_FREQ.id_;
    const auto& slope_id   = low_cut ? GraphicEq::LOW_CUT_SLOPE.id_ : GraphicEq::HIGH_CUT_SLOPE.id_;
    const auto& enabled_id = low_cut ? GraphicEq::LOW_CUT_ENABLED.id_ : GraphicEq::HIGH_CUT_ENABLED.id_;

    powerButton     = std::make_unique< PowerButton >();
    freqSlider      = std::make_unique< CustomRotaryControl >(apvts.getParameter(freq_id), "Hz");
    slopeSlider     = std::make_unique< CustomLinearSlider >(apvts.getParameter(slope_id));
    powerAttachment = std::make_unique< ButtonAttachment >(apvts, enabled_id, *powerButton);
    freqAttachment  = std::make_unique< SliderAttachment >(apvts, freq_id, *freqSlider);
    slopeAttachment = std::make_unique< SliderAttachment >(apvts, slope_id, *slopeSlider);

    powerButton->onClick      = [this]() { setEnablement(); };
    freqSlider->onValueChange = [this]() { repaint(); };

    addAndMakeVisible(*powerButton);
    addAndMakeVisible(*freqSlider);
    addAndMakeVisible(*slopeSlider);

    setEnablement();
}

/*---------------------------------------------------------------------------
**
*/
void
CutControl::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    g.setFont(GraphicEq::FONT);
    auto textHeight = g.getCurrentFont().getHeight();
    g.setColour(ColourPalette::Text);

    g.drawFittedText(chainPosition == FilterUtils::ChainPositions::LowCut ? "LC" : "HC",
                     bounds.getRight() - (textHeight * 2),
                     textHeight * 0.5,
                     textHeight * 2,
                     textHeight,
                     juce::Justification::centred,
                     1);

    auto sliderQuarter = slopeSlider->getWidth() * 0.25;
    auto sliderX       = slopeSlider->getX();
    auto sliderBottom  = slopeSlider->getBottom();

    g.drawFittedText("dB/Oct",
                     sliderX,
                     slopeSlider->getY() - textHeight - 4,
                     slopeSlider->getWidth(),
                     textHeight,
                     juce::Justification::centred,
                     1);

    for (auto i = 0; i < 4; ++i) {
        g.drawFittedText(juce::String((i + 1) * 12),
                         sliderX + (sliderQuarter * i),
                         sliderBottom + 4,
                         sliderQuarter,
                         textHeight,
                         juce::Justification::centred,
                         1);
    }

    // Values
    auto valueRectHeight = bounds.getHeight() * 0.4;
    auto valueRect = juce::Rectangle< int >(0, bounds.getBottom() - valueRectHeight, bounds.getWidth(), valueRectHeight);

    auto vCX = valueRect.getCentreX();
    auto vY  = valueRect.getY();
    auto vW  = valueRect.getWidth() * 0.5;
    auto vH  = valueRect.getHeight() / 3;

    g.drawFittedText("Hz:", 0, vY + vH, vW, vH, juce::Justification::centred, 1);
    g.drawFittedText(juce::String(freqSlider->getValue()), vCX, vY + vH, vW, vH, juce::Justification::centred, 1);
}

/*---------------------------------------------------------------------------
**
*/
void
CutControl::resized()
{
    auto bounds   = getLocalBounds();
    auto diameter = (bounds.getWidth() * 0.55);
    auto padding  = GraphicEq::FONT.getHeight();

    powerButton->setBounds(6, 6, 16, 16);
    freqSlider->setBounds(bounds.getCentreX() - (diameter * 0.5),
                          bounds.getCentreX() - (diameter * 0.5),
                          diameter,
                          diameter);
    slopeSlider->setBounds(padding, freqSlider->getBottom() + (padding * 3), bounds.getWidth() - (padding * 2), padding);
}

/*---------------------------------------------------------------------------
**
*/
void
CutControl::setEnablement()
{
    bool toggleState = powerButton->getToggleState();

    freqSlider->setEnabled(toggleState);
    slopeSlider->setEnabled(toggleState);

    float dim = 0.2f;
    freqSlider->setAlpha(toggleState ? 1.f : dim);
    slopeSlider->setAlpha(toggleState ? 1.f : dim);
}

/*---------------------------------------------------------------------------
** End of File
*/
