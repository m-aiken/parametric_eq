#include "ColourPalette.h"
#include "EqProperties.h"
#include "ResponseCurve.h"

/*---------------------------------------------------------------------------
**
*/
Node::Node(juce::AudioProcessorValueTreeState& _apvts, int _bandNum)
    : paramAttachment(_apvts, EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, _bandNum), *this)
{
}

/*---------------------------------------------------------------------------
**
*/
void
Node::paint(juce::Graphics& g)
{
    g.setColour(getToggleState() ? ColourPalette::NodeOn : ColourPalette::NodeOff);
    g.fillEllipse(getLocalBounds().toFloat());
}

/*---------------------------------------------------------------------------
**
*/
ResponseCurve::ResponseCurve(juce::AudioProcessorValueTreeState& _apvts, double _sampleRate)
    : sampleRate(_sampleRate)
    , apvts(_apvts)
{
    for (size_t i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        peakNodes.at(i) = std::make_unique< Node >(_apvts, i);
    }

    auto assignBoolParam = [&](auto& target, auto& paramName) {
        if (auto* param = dynamic_cast< juce::AudioParameterBool* >(apvts.getParameter(paramName)); param != nullptr) {
            target = param;
        }
    };

    auto assignFloatParam = [&](auto& target, auto& paramName) {
        if (auto* param = dynamic_cast< juce::AudioParameterFloat* >(apvts.getParameter(paramName)); param != nullptr) {
            target = param;
        }
    };

    auto assignChoiceParam = [&](auto& target, auto& paramName) {
        if (auto* param = dynamic_cast< juce::AudioParameterChoice* >(apvts.getParameter(paramName)); param != nullptr) {
            target = param;
        }
    };

    assignBoolParam(lowCutEnabledParam, GraphicEq::LOW_CUT_ENABLED.id_);
    assignFloatParam(lowCutFreqParam, GraphicEq::LOW_CUT_FREQ.id_);
    assignChoiceParam(lowCutSlopeParam, GraphicEq::LOW_CUT_SLOPE.id_);

    assignBoolParam(highCutEnabledParam, GraphicEq::HIGH_CUT_ENABLED.id_);
    assignFloatParam(highCutFreqParam, GraphicEq::HIGH_CUT_FREQ.id_);
    assignChoiceParam(highCutSlopeParam, GraphicEq::HIGH_CUT_SLOPE.id_);

    for (size_t i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        assignFloatParam(peakBands.at(i).peakFreqParam,
                         EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i));
        assignFloatParam(peakBands.at(i).peakGainParam,
                         EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i));
        assignFloatParam(peakBands.at(i).peakQParam,
                         EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i));
        assignBoolParam(peakBands.at(i).peakEnabledParam,
                        EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, i));

        addAndMakeVisible(*peakNodes.at(i));

        nodeCoordinates.at(i).setXY(0, 0);

        xValues.at(i).referTo(
            apvts.getParameterAsValue(EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i)));
        yValues.at(i).referTo(
            apvts.getParameterAsValue(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i)));
    }

    activeNode = 0;

    addListeners();
    updateMonoChain();
    startTimerHz(60);
}

/*---------------------------------------------------------------------------
**
*/
ResponseCurve::~ResponseCurve()
{
    stopTimer();
    removeListeners();
}

/*---------------------------------------------------------------------------
**
*/
void
ResponseCurve::paint(juce::Graphics& g)
{
    auto bounds           = getLocalBounds();
    auto boundsWidth      = bounds.getWidth();
    auto boundsX          = bounds.getX();
    auto responseCurveMin = bounds.getBottom();
    auto responseCurveMax = bounds.getY();

    std::array< std::vector< double >, 9 > magnitudes = getMagnitudes(boundsWidth);
    std::array< juce::Path, 9 >            paths;

    auto mapFilterGainRangeToAnalyzerBounds = [&](double magnitude) {
        return juce::jmap< float >(magnitude, GraphicEq::MIN_DB, GraphicEq::MAX_DB, responseCurveMin, responseCurveMax);
    };

    // Plotting the paths for each band
    // These need to be closed paths starting and ending at 0 for the fill effect to look correct
    for (size_t bandIdx = 0; bandIdx < paths.size() - 1; ++bandIdx) {
        paths.at(bandIdx).startNewSubPath(boundsX, mapFilterGainRangeToAnalyzerBounds(0.0));

        for (int i = 1; i < boundsWidth - 1; ++i) {
            paths.at(bandIdx).lineTo(boundsX + i, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(bandIdx).at(i)));
        }

        paths.at(bandIdx).lineTo(boundsWidth - 1, mapFilterGainRangeToAnalyzerBounds(0.0));
        paths.at(bandIdx).closeSubPath();
    }

    // Drawing the bolder outline
    g.setColour(ColourPalette::ResponseCurve);
    for (size_t pathIdx = 0; pathIdx < paths.size() - 1; ++pathIdx) {
        g.strokePath(paths.at(pathIdx), juce::PathStrokeType(1.f));
    }

    // Drawing the faded fill
    g.setColour(ColourPalette::ResponseCurve.withAlpha(0.2f));
    for (size_t pathIdx = 0; pathIdx < paths.size() - 1; ++pathIdx) {
        g.fillPath(paths.at(pathIdx));
    }

    // Plotting the path for total filter response
    // This should NOT be closed so that it's just a single line
    for (size_t bandIdx = 0; bandIdx < paths.size(); ++bandIdx) {
        paths.at(bandIdx).startNewSubPath(boundsX, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(bandIdx).at(0)));

        for (int i = 1; i < boundsWidth; ++i) {
            paths.at(bandIdx).lineTo(boundsX + i, mapFilterGainRangeToAnalyzerBounds(magnitudes.at(bandIdx).at(i)));
        }
    }

    g.setColour(ColourPalette::ResponseCurveAvg);
    g.strokePath(paths.at(paths.size() - 1), juce::PathStrokeType(1.f));

    // Drawing the nodes
    for (size_t i = 0; i < peakNodes.size(); ++i) {
        if (peakBands.at(i).peakFreqParam == nullptr || peakNodes.at(i) == nullptr) {
            continue;
        }

        auto normalizedFrequency = juce::mapFromLog10< float >(peakBands.at(i).peakFreqParam->get(),
                                                               GraphicEq::MIN_HZ,
                                                               GraphicEq::MAX_HZ);

        auto xRaw = boundsX + static_cast< int >(std::floor(boundsWidth * normalizedFrequency));
        // Clamp the upper bounds for the x coordinate so that it can't go out of the vector bounds.
        auto nodeX = juce::jmin< int >(xRaw, boundsWidth - nodeRadius);
        auto nodeY = mapFilterGainRangeToAnalyzerBounds(magnitudes.at(i).at(nodeX - boundsX));

        peakNodes.at(i)->setBounds(nodeX - nodeRadius, nodeY - nodeRadius, nodeDiameter, nodeDiameter);
        nodeCoordinates.at(i).setXY(nodeX, static_cast< int >(std::floor(nodeY)));
    }

    // Vertical lines on both ends of the container to hide the edge of the response curve
    g.setColour(ColourPalette::MainBackground);
    auto lineThickness = 3.f;

    g.drawLine(0, 0, 0, bounds.getBottom(), lineThickness);

    g.drawLine(boundsWidth - 1, 0, boundsWidth - 1, bounds.getBottom(), lineThickness);
}

/*---------------------------------------------------------------------------
**
*/
void
ResponseCurve::mouseDrag(const juce::MouseEvent& event)
{
    auto xCoord = event.position.getX();
    auto yCoord = event.position.getY();

    auto bounds       = getLocalBounds();
    auto boundsWidth  = bounds.getWidth();
    auto boundsHeight = bounds.getHeight();

    if (xCoord >= 0 && xCoord <= boundsWidth && yCoord >= 0 && yCoord <= boundsHeight) {
        auto xFrequency = mapToLog10< float >(static_cast< float >(xCoord) / boundsWidth,
                                              GraphicEq::MIN_HZ,
                                              GraphicEq::MAX_HZ);

        auto yDecibels = juce::jmap< float >(yCoord, 0, boundsHeight, GraphicEq::MAX_DB, GraphicEq::MIN_DB);

        size_t closestNode      = 0;
        auto   shortestDistance = boundsWidth;
        for (size_t i = 0; i < nodeCoordinates.size(); ++i) {
            auto xDistance = std::abs(nodeCoordinates.at(i).getX() - xCoord);
            if (xDistance < shortestDistance) {
                closestNode      = i;
                shortestDistance = xDistance;
            }
        }

        if (peakNodes.at(closestNode) != nullptr && peakNodes.at(closestNode)->getToggleState()) {
            xValues.at(closestNode) = xFrequency;
            yValues.at(closestNode) = yDecibels;

            activeNode = closestNode;
        }
    }
}

/*---------------------------------------------------------------------------
**
*/
void
ResponseCurve::updateMonoChain()
{
    if (lowCutEnabledParam == nullptr || highCutEnabledParam == nullptr) {
        return;
    }

    FilterUtils::updateBandEnablements(monoChain, lowCutEnabledParam, highCutEnabledParam, peakBands);

    if (lowCutEnabledParam->get()) {
        auto lowCutCoefficients = FilterUtils::makeHighPassFilter(lowCutFreqParam, lowCutSlopeParam, sampleRate);
        FilterUtils::updateCutCoefficients(monoChain,
                                           FilterUtils::ChainPositions::LowCut,
                                           lowCutCoefficients,
                                           lowCutSlopeParam);
    }

    if (highCutEnabledParam->get()) {
        auto highCutCoefficients = FilterUtils::makeLowPassFilter(highCutFreqParam, highCutSlopeParam, sampleRate);
        FilterUtils::updateCutCoefficients(monoChain,
                                           FilterUtils::ChainPositions::HighCut,
                                           highCutCoefficients,
                                           highCutSlopeParam);
    }

    for (size_t i = 0; i < peakBands.size(); ++i) {
        if (peakBands.at(i).peakEnabledParam != nullptr && peakBands.at(i).peakEnabledParam->get()) {
            FilterUtils::updatePeakCoefficients(monoChain,
                                                static_cast< FilterUtils::ChainPositions >(i + 1),
                                                peakBands.at(i).peakFreqParam,
                                                peakBands.at(i).peakQParam,
                                                peakBands.at(i).peakGainParam,
                                                sampleRate);
        }
    }
}

/*---------------------------------------------------------------------------
**
*/
std::array< std::vector< double >, 9 >
ResponseCurve::getMagnitudes(int boundsWidth)
{
    std::array< std::vector< double >, 9 > magnitudes;
    for (auto& mag : magnitudes) {
        mag.resize(boundsWidth);
    }

    enum MagIdx { P0, P1, P2, P3, P4, P5, LC, HC, ALL };

    std::array< juce::dsp::IIR::Filter< float >::CoefficientsPtr, GraphicEq::NUM_PEAK_BANDS > peakCoefficients;

    peakCoefficients[0] = monoChain.get< FilterUtils::ChainPositions::Peak_0 >().coefficients;
    peakCoefficients[1] = monoChain.get< FilterUtils::ChainPositions::Peak_1 >().coefficients;
    peakCoefficients[2] = monoChain.get< FilterUtils::ChainPositions::Peak_2 >().coefficients;
    peakCoefficients[3] = monoChain.get< FilterUtils::ChainPositions::Peak_3 >().coefficients;
    peakCoefficients[4] = monoChain.get< FilterUtils::ChainPositions::Peak_4 >().coefficients;
    peakCoefficients[5] = monoChain.get< FilterUtils::ChainPositions::Peak_5 >().coefficients;

    auto& lowCut  = monoChain.get< FilterUtils::ChainPositions::LowCut >();
    auto& highCut = monoChain.get< FilterUtils::ChainPositions::HighCut >();

    for (auto i = 0; i < boundsWidth; ++i) {
        std::array< double, GraphicEq::NUM_PEAK_BANDS > peakMags;
        peakMags.fill(1.0);

        double magLC  = 1.0;
        double magHC  = 1.0;
        double magALL = 1.0;

        auto freq = mapToLog10< double >(static_cast< double >(i) / boundsWidth, GraphicEq::MIN_HZ, GraphicEq::MAX_HZ);

        for (size_t peakBand = 0; peakBand < GraphicEq::NUM_PEAK_BANDS; ++peakBand) {
            if (peakBands.at(peakBand).peakEnabledParam != nullptr && peakCoefficients.at(peakBand) != nullptr
                && peakBands.at(peakBand).peakEnabledParam->get()) {
                peakMags.at(peakBand) *= peakCoefficients.at(peakBand)->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= peakCoefficients.at(peakBand)->getMagnitudeForFrequency(freq, sampleRate);
            }
        }

        if (lowCutEnabledParam != nullptr && lowCutEnabledParam->get()) {
            if (!lowCut.isBypassed< 0 >() && lowCut.get< 0 >().coefficients != nullptr) {
                magHC *= lowCut.get< 0 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= lowCut.get< 0 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!lowCut.isBypassed< 1 >() && lowCut.get< 1 >().coefficients != nullptr) {
                magHC *= lowCut.get< 1 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= lowCut.get< 1 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!lowCut.isBypassed< 2 >() && lowCut.get< 2 >().coefficients != nullptr) {
                magHC *= lowCut.get< 2 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= lowCut.get< 2 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!lowCut.isBypassed< 3 >() && lowCut.get< 3 >().coefficients != nullptr) {
                magHC *= lowCut.get< 3 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= lowCut.get< 3 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
        }

        if (highCutEnabledParam != nullptr && highCutEnabledParam->get()) {
            if (!highCut.isBypassed< 0 >() && highCut.get< 0 >().coefficients != nullptr) {
                magHC *= highCut.get< 0 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= highCut.get< 0 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!highCut.isBypassed< 1 >() && highCut.get< 1 >().coefficients != nullptr) {
                magHC *= highCut.get< 1 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= highCut.get< 1 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!highCut.isBypassed< 2 >() && highCut.get< 2 >().coefficients != nullptr) {
                magHC *= highCut.get< 2 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= highCut.get< 2 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
            if (!highCut.isBypassed< 3 >() && highCut.get< 3 >().coefficients != nullptr) {
                magHC *= highCut.get< 3 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
                magALL *= highCut.get< 3 >().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            }
        }

        for (size_t peakBand = 0; peakBand < GraphicEq::NUM_PEAK_BANDS; ++peakBand) {
            magnitudes.at(peakBand).at(i) = juce::Decibels::gainToDecibels(peakMags.at(peakBand));
        }

        magnitudes.at(MagIdx::LC).at(i)  = juce::Decibels::gainToDecibels(magLC);
        magnitudes.at(MagIdx::HC).at(i)  = juce::Decibels::gainToDecibels(magHC);
        magnitudes.at(MagIdx::ALL).at(i) = juce::Decibels::gainToDecibels(magALL);
    }

    return magnitudes;
}

/*---------------------------------------------------------------------------
**
*/
void
ResponseCurve::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged.set(true);
}

/*---------------------------------------------------------------------------
**
*/
void
ResponseCurve::timerCallback()
{
    if (parametersChanged.compareAndSetBool(false, true)) {
        updateMonoChain();
        repaint();
    }
}

/*---------------------------------------------------------------------------
**
*/
void
ResponseCurve::addListeners()
{
    if (!allControlPtrsValid()) {
        return;
    }

    apvts.getParameter(GraphicEq::LOW_CUT_FREQ.id_)->addListener(this);
    apvts.getParameter(GraphicEq::LOW_CUT_SLOPE.id_)->addListener(this);
    apvts.getParameter(GraphicEq::LOW_CUT_ENABLED.id_)->addListener(this);

    apvts.getParameter(GraphicEq::HIGH_CUT_FREQ.id_)->addListener(this);
    apvts.getParameter(GraphicEq::HIGH_CUT_SLOPE.id_)->addListener(this);
    apvts.getParameter(GraphicEq::HIGH_CUT_ENABLED.id_)->addListener(this);

    for (size_t i = 0; i < peakBands.size(); ++i) {
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i))->addListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i))->addListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i))->addListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, i))->addListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
ResponseCurve::removeListeners()
{
    if (!allControlPtrsValid()) {
        return;
    }

    apvts.getParameter(GraphicEq::LOW_CUT_FREQ.id_)->removeListener(this);
    apvts.getParameter(GraphicEq::LOW_CUT_SLOPE.id_)->removeListener(this);
    apvts.getParameter(GraphicEq::LOW_CUT_ENABLED.id_)->removeListener(this);

    apvts.getParameter(GraphicEq::HIGH_CUT_FREQ.id_)->removeListener(this);
    apvts.getParameter(GraphicEq::HIGH_CUT_SLOPE.id_)->removeListener(this);
    apvts.getParameter(GraphicEq::HIGH_CUT_ENABLED.id_)->removeListener(this);

    for (size_t i = 0; i < peakBands.size(); ++i) {
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i))
            ->removeListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i))->removeListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i))
            ->removeListener(this);
        apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, i))
            ->removeListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
bool
ResponseCurve::allControlPtrsValid()
{
    bool ok = true;

    ok &= apvts.getParameter(GraphicEq::LOW_CUT_FREQ.id_) != nullptr;
    ok &= apvts.getParameter(GraphicEq::LOW_CUT_SLOPE.id_) != nullptr;
    ok &= apvts.getParameter(GraphicEq::LOW_CUT_ENABLED.id_) != nullptr;

    ok &= apvts.getParameter(GraphicEq::HIGH_CUT_FREQ.id_) != nullptr;
    ok &= apvts.getParameter(GraphicEq::HIGH_CUT_SLOPE.id_) != nullptr;
    ok &= apvts.getParameter(GraphicEq::HIGH_CUT_ENABLED.id_) != nullptr;

    for (size_t i = 0; i < peakBands.size(); ++i) {
        ok &= apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i))
              != nullptr;
        ok &= apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i)) != nullptr;
        ok &= apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i)) != nullptr;
        ok &= apvts.getParameter(EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, i)) != nullptr;
    }

    return ok;
}

/*---------------------------------------------------------------------------
** End of File
*/
