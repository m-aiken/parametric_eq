#include "ColourPalette.h"
#include "EqProperties.h"
#include "PeakControl.h"

/*---------------------------------------------------------------------------
**
*/
PeakControl::PeakControl(juce::AudioProcessorValueTreeState& apvts, const int _bandNum)
    : bandNum(_bandNum)
{
    powerButton      = std::make_unique< PowerButton >();
    buttonAttachment = std::make_unique< juce::AudioProcessorValueTreeState::ButtonAttachment >(
        apvts,
        EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, bandNum),
        *powerButton);

    auto freqParam = apvts.getParameter(
        EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, bandNum));
    jassert(freqParam != nullptr);
    freqSlider = std::make_unique< CustomRotaryControl >(*freqParam, "Hz");

    auto gainParam = apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, bandNum));
    jassert(gainParam != nullptr);
    gainSlider = std::make_unique< CustomRotaryControl >(*gainParam, "dB");

    auto qParam = apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, bandNum));
    jassert(qParam != nullptr);
    qSlider = std::make_unique< CustomRotaryControl >(*qParam, "Q");

    freqAttachment = std::make_unique< juce::AudioProcessorValueTreeState::SliderAttachment >(
        apvts,
        EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, bandNum),
        *freqSlider);
    gainAttachment = std::make_unique< juce::AudioProcessorValueTreeState::SliderAttachment >(
        apvts,
        EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, bandNum),
        *gainSlider);
    qAttachment = std::make_unique< juce::AudioProcessorValueTreeState::SliderAttachment >(
        apvts,
        EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, bandNum),
        *qSlider);

    powerButton->onClick      = [this]() { setEnablement(); };
    freqSlider->onValueChange = [this]() { repaint(); };
    gainSlider->onValueChange = [this]() { repaint(); };
    qSlider->onValueChange    = [this]() { repaint(); };

    addAndMakeVisible(*powerButton);
    addAndMakeVisible(*freqSlider);
    addAndMakeVisible(*gainSlider);
    addAndMakeVisible(*qSlider);

    setEnablement();
}

/*---------------------------------------------------------------------------
**
*/
void
PeakControl::paint(juce::Graphics& g)
{
    // TODO set a proper colour for nodeFocusOnColour
    auto nodeFocusOnColour  = ColourPalette::getColourV2(ColourPalette::PeakBandFocus);
    auto nodeFocusOffColour = ColourPalette::getColourV2(ColourPalette::MainBackground);
    g.fillAll(nodeIsActive ? nodeFocusOnColour : nodeFocusOffColour);

    auto bounds = getLocalBounds();

    g.setFont(GraphicEqGlobals::getFont());
    auto textHeight = g.getCurrentFont().getHeight();
    g.setColour(ColourPalette::getColourV2(ColourPalette::Text));

    g.drawFittedText("P" + juce::String(bandNum),
                     bounds.getRight() - (textHeight * 2),
                     textHeight * 0.5,
                     textHeight * 2,
                     textHeight,
                     juce::Justification::centred,
                     1);

    // Values
    auto labelRectHeight = bounds.getHeight() * 0.4;
    auto labelRect = juce::Rectangle< int >(0, bounds.getBottom() - labelRectHeight, bounds.getWidth(), labelRectHeight);

    auto centreX     = labelRect.getCentreX();
    auto labelRectY  = labelRect.getY();
    auto labelWidth  = labelRect.getWidth() * 0.5;
    auto labelHeight = labelRect.getHeight() / 3;
    auto padding     = GraphicEqGlobals::getFont().getHeight();

    auto         dbValue  = (gainSlider != nullptr) ? gainSlider->getValue() : 0;
    juce::String dbPrefix = dbValue > 0 ? "+" : "";

    using ParamPair = std::pair< juce::String, juce::String >;
    std::array< ParamPair, 3 > paramValues {
        std::pair { "Hz:", (freqSlider != nullptr) ? juce::String(freqSlider->getValue()) : "**" },
        std::pair { "dB:", dbPrefix + juce::String(dbValue) },
        std::pair { "Q:", juce::String((qSlider != nullptr) ? qSlider->getValue() : 1.0) }
    };

    for (size_t i = 0; i < paramValues.size(); ++i) {
        // label
        g.drawFittedText(paramValues.at(i).first,
                         0,
                         labelRectY + (labelHeight * i),
                         labelWidth - padding,
                         labelHeight,
                         juce::Justification::centredRight,
                         1);

        // value
        g.drawFittedText(paramValues.at(i).second,
                         centreX + padding,
                         labelRectY + (labelHeight * i),
                         labelWidth - padding,
                         labelHeight,
                         juce::Justification::centredLeft,
                         1);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
PeakControl::resized()
{
    if (powerButton == nullptr || freqSlider == nullptr || gainSlider == nullptr || qSlider == nullptr) {
        return;
    }

    auto bounds         = getLocalBounds();
    auto rotaryBounds   = juce::Rectangle< int >(0, 0, bounds.getWidth(), bounds.getHeight() * 0.5);
    auto padding        = 2;
    auto rotaryDiameter = (rotaryBounds.getWidth() * 0.5) - padding;

    powerButton->setBounds(6, 6, 16, 16);

    freqSlider->setBounds(padding, rotaryDiameter * 0.5, rotaryDiameter, rotaryDiameter);

    gainSlider->setBounds(padding, rotaryBounds.getCentreY() + (rotaryDiameter * 0.5), rotaryDiameter, rotaryDiameter);

    qSlider->setBounds(rotaryBounds.getRight() - rotaryDiameter - padding,
                       rotaryBounds.getCentreY(),
                       rotaryDiameter,
                       rotaryDiameter);
}

/*---------------------------------------------------------------------------
**
*/
void
PeakControl::setNodeIsActive(bool activeState)
{
    nodeIsActive = activeState;
    repaint();
}

/*---------------------------------------------------------------------------
**
*/
void
PeakControl::setEnablement()
{
    if (powerButton == nullptr || freqSlider == nullptr || gainSlider == nullptr || qSlider == nullptr) {
        return;
    }

    bool toggleState = powerButton->getToggleState();

    freqSlider->setEnabled(toggleState);
    gainSlider->setEnabled(toggleState);
    qSlider->setEnabled(toggleState);

    float dim = 0.2f;
    freqSlider->setAlpha(toggleState ? 1.f : dim);
    gainSlider->setAlpha(toggleState ? 1.f : dim);
    qSlider->setAlpha(toggleState ? 1.f : dim);
}

/*---------------------------------------------------------------------------
** End of File
*/
