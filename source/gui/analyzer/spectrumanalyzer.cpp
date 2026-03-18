#include "AnalyzerProperties.h"
#include "SpectrumAnalyzer.h"

/*---------------------------------------------------------------------------
**
*/
SpectrumAnalyzer::SpectrumAnalyzer(double                                        _sampleRate,
                                   MonoBufferFifo< juce::AudioBuffer< float > >& leftScsf,
                                   MonoBufferFifo< juce::AudioBuffer< float > >& rightScsf,
                                   juce::AudioProcessorValueTreeState&           apvts,
                                   const juce::Colour&                           fillColour)
    : sampleRate(_sampleRate)
    , colour(fillColour)
    , leftPathProducer(_sampleRate, leftScsf)
    , rightPathProducer(_sampleRate, rightScsf)
    , analyzerOrderParamListener(*apvts.getParameter(AnalyzerProperties::getAnalyzerParams().at(
                                     AnalyzerProperties::ParamNames::Analyzer_Points)),
                                 [this](const auto& newOrder) { updateOrder(newOrder); })
    , analyzerDecayRateParamListener(*apvts.getParameter(AnalyzerProperties::getAnalyzerParams().at(
                                         AnalyzerProperties::ParamNames::Analyzer_Decay_Rate)),
                                     [this](const auto& decayRate) { updateDecayRate(decayRate); })
{
    auto orderParam = apvts.getParameter(
        AnalyzerProperties::getAnalyzerParams().at(AnalyzerProperties::ParamNames::Analyzer_Points));

    if (orderParam != nullptr) {
        updateOrder(orderParam->getValue());
    }

    auto decayRateParam = apvts.getParameter(
        AnalyzerProperties::getAnalyzerParams().at(AnalyzerProperties::ParamNames::Analyzer_Decay_Rate));

    if (decayRateParam != nullptr) {
        updateDecayRate(decayRateParam->convertFrom0to1(decayRateParam->getValue()));
    }

    leftPathProducer.toggleProcessing(true);
    rightPathProducer.toggleProcessing(true);

    animate();
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyzer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    leftAnalyzerPath.applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));
    rightAnalyzerPath.applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));

    g.setColour(colour);
    g.fillPath(leftAnalyzerPath);
    g.fillPath(rightAnalyzerPath);
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyzer::resized()
{
    auto bounds = getLocalBounds();
    leftPathProducer.setFFTRectBounds(bounds.toFloat());
    rightPathProducer.setFFTRectBounds(bounds.toFloat());
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyzer::timerCallback()
{
    if (!active) {
        leftAnalyzerPath.clear();
        rightAnalyzerPath.clear();
        stopTimer();
    }
    else {
        if (leftPathProducer.getNumAvailableForReading() > 0) {
            while (leftPathProducer.pull(leftAnalyzerPath)) {
            }  // get most recent
        }

        if (rightPathProducer.getNumAvailableForReading() > 0) {
            while (rightPathProducer.pull(rightAnalyzerPath)) {
            }  // get most recent
        }
    }

    repaint();
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyzer::setActive(bool activeState)
{
    active = activeState;
    if (active && !isTimerRunning()) {
        animate();
    }
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyzer::updateDecayRate(float decayRate)
{
    leftPathProducer.setDecayRate(decayRate);
    rightPathProducer.setDecayRate(decayRate);
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyzer::updateOrder(float value)
{
    auto denormalizedVal = static_cast< int >(std::floor(juce::jmap< float >(value, 0.f, 1.f, 11.f, 13.f)));
    leftPathProducer.changeOrder(static_cast< GraphicEq::FFTOrder >(denormalizedVal));
    rightPathProducer.changeOrder(static_cast< GraphicEq::FFTOrder >(denormalizedVal));
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyzer::animate()
{
    startTimerHz(60);
}

/*---------------------------------------------------------------------------
** End of File
*/
