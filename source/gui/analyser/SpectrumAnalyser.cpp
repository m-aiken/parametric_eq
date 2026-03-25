#include "SpectrumAnalyser.h"

/*---------------------------------------------------------------------------
**
*/
SpectrumAnalyser::SpectrumAnalyser(double                                        _sampleRate,
                                   MonoBufferFifo< juce::AudioBuffer< float > >& leftScsf,
                                   MonoBufferFifo< juce::AudioBuffer< float > >& rightScsf,
                                   juce::AudioProcessorValueTreeState&           apvts,
                                   const juce::Colour&                           fillColour)
    : sampleRate(_sampleRate)
    , colour(fillColour)
    , leftPathProducer(_sampleRate, leftScsf)
    , rightPathProducer(_sampleRate, rightScsf)
    , analyzerOrderParamListener(*apvts.getParameter(GraphicEq::ANALYSER_POINTS.id_),
                                 [this](const auto& newOrder) { updateOrder(newOrder); })
{
    auto orderParam = apvts.getParameter(GraphicEq::ANALYSER_POINTS.id_);

    if (orderParam != nullptr) {
        updateOrder(orderParam->getValue());
    }

    leftPathProducer.toggleProcessing(true);
    rightPathProducer.toggleProcessing(true);

    animate();
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyser::paint(juce::Graphics& g)
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
SpectrumAnalyser::resized()
{
    auto bounds = getLocalBounds();
    leftPathProducer.setFFTRectBounds(bounds.toFloat());
    rightPathProducer.setFFTRectBounds(bounds.toFloat());
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyser::timerCallback()
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
SpectrumAnalyser::setActive(bool activeState)
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
SpectrumAnalyser::updateOrder(float value)
{
    auto denormalizedVal = static_cast< int >(std::floor(juce::jmap< float >(value, 0.f, 1.f, 11.f, 13.f)));
    leftPathProducer.changeOrder(static_cast< GraphicEq::FFTOrder >(denormalizedVal));
    rightPathProducer.changeOrder(static_cast< GraphicEq::FFTOrder >(denormalizedVal));
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyser::animate()
{
    startTimerHz(60);
}

/*---------------------------------------------------------------------------
** End of File
*/
