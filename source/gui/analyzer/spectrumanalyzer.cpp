#include "AnalyzerProperties.h"
#include "SpectrumAnalyzer.h"

//==============================================================================
SpectrumAnalyzer::SpectrumAnalyzer(double                                    _sampleRate,
                                   MonoBufferFifo<juce::AudioBuffer<float>>& leftScsf,
                                   MonoBufferFifo<juce::AudioBuffer<float>>& rightScsf,
                                   juce::AudioProcessorValueTreeState&       apvts,
                                   const juce::Colour&                       fillColour)
    : sampleRate(_sampleRate)
    , colour(fillColour)
{
    leftAnalyzerPath  = std::make_unique<juce::Path>();
    rightAnalyzerPath = std::make_unique<juce::Path>();

    leftPathProducer  = std::make_unique<PathProducer>(_sampleRate, leftScsf);
    rightPathProducer = std::make_unique<PathProducer>(_sampleRate, rightScsf);

    const auto& params = AnalyzerProperties::getAnalyzerParams();

    auto initListener = [&](auto& listener, const auto& paramName, const auto& lambda) {
        auto param = apvts.getParameter(params.at(paramName));
        if (param != nullptr) {
            listener = std::make_unique<ParamListener<float>>(*param, lambda);
        }
    };

    initListener(analyzerOrderParamListener,
                 AnalyzerProperties::ParamNames::Analyzer_Points,
                 [this](const auto& newOrder) { updateOrder(newOrder); });

    initListener(analyzerDecayRateParamListener,
                 AnalyzerProperties::ParamNames::Analyzer_Decay_Rate,
                 [this](const auto& decayRate) { updateDecayRate(decayRate); });

    auto orderParam = apvts.getParameter(params.at(AnalyzerProperties::ParamNames::Analyzer_Points));
    if (orderParam != nullptr) {
        updateOrder(orderParam->getValue());
    }

    auto decayRateParam = apvts.getParameter(params.at(AnalyzerProperties::ParamNames::Analyzer_Decay_Rate));
    if (decayRateParam != nullptr) {
        updateDecayRate(decayRateParam->convertFrom0to1(decayRateParam->getValue()));
    }

    leftPathProducer->toggleProcessing(true);
    rightPathProducer->toggleProcessing(true);

    animate();
}

void SpectrumAnalyzer::paint(juce::Graphics& g)
{
    if (leftAnalyzerPath == nullptr || rightAnalyzerPath == nullptr) {
        return;
    }

    auto bounds = getLocalBounds();

    leftAnalyzerPath->applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));
    rightAnalyzerPath->applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));

    g.setColour(colour);
    g.fillPath(*leftAnalyzerPath);
    g.fillPath(*rightAnalyzerPath);
}

void SpectrumAnalyzer::resized()
{
    if (leftPathProducer == nullptr || rightPathProducer == nullptr) {
        return;
    }

    auto bounds = getLocalBounds();
    leftPathProducer->setFFTRectBounds(bounds.toFloat());
    rightPathProducer->setFFTRectBounds(bounds.toFloat());
}

void SpectrumAnalyzer::timerCallback()
{
    if (leftAnalyzerPath == nullptr || rightAnalyzerPath == nullptr || leftPathProducer == nullptr || rightPathProducer == nullptr) {
        return;
    }

    if (!active) {
        leftAnalyzerPath->clear();
        rightAnalyzerPath->clear();
        stopTimer();
    } else {
        if (leftPathProducer->getNumAvailableForReading() > 0) {
            while (leftPathProducer->pull(*leftAnalyzerPath)) {
            } // get most recent
        }

        if (rightPathProducer->getNumAvailableForReading() > 0) {
            while (rightPathProducer->pull(*rightAnalyzerPath)) {
            } // get most recent
        }
    }

    repaint();
}

void SpectrumAnalyzer::setActive(bool activeState)
{
    active = activeState;
    if (active && !isTimerRunning()) {
        animate();
    }
}

void SpectrumAnalyzer::updateDecayRate(float decayRate)
{
    if (leftPathProducer == nullptr || rightPathProducer == nullptr) {
        return;
    }

    leftPathProducer->setDecayRate(decayRate);
    rightPathProducer->setDecayRate(decayRate);
}

void SpectrumAnalyzer::updateOrder(float value)
{
    if (leftPathProducer == nullptr || rightPathProducer == nullptr) {
        return;
    }

    auto denormalizedVal = static_cast<int>(std::floor(juce::jmap<float>(value, 0.f, 1.f, 11.f, 13.f)));
    leftPathProducer->changeOrder(static_cast<GraphicEqGlobals::FFTOrder>(denormalizedVal));
    rightPathProducer->changeOrder(static_cast<GraphicEqGlobals::FFTOrder>(denormalizedVal));
}

void SpectrumAnalyzer::animate()
{
    startTimerHz(60);
}
