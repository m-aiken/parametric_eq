#include "AnalyzerContainer.h"
#include "ColourPalette.h"

/*---------------------------------------------------------------------------
**
*/
AnalyzerContainer::AnalyzerContainer(juce::AudioProcessorValueTreeState&           apvts,
                                     double                                        sampleRate,
                                     MonoBufferFifo< juce::AudioBuffer< float > >& preEqBufferL,
                                     MonoBufferFifo< juce::AudioBuffer< float > >& preEqBufferR,
                                     MonoBufferFifo< juce::AudioBuffer< float > >& postEqBufferL,
                                     MonoBufferFifo< juce::AudioBuffer< float > >& postEqBufferR)
{
    backgroundGrid         = std::make_unique< SpectrumGrid >();
    spectrumAnalyzerPreEq  = std::make_unique< SpectrumAnalyzer >(sampleRate,
                                                                 preEqBufferL,
                                                                 preEqBufferR,
                                                                 apvts,
                                                                 ColourPalette::AnalyzerPreEq.withAlpha(0.1f));
    spectrumAnalyzerPostEq = std::make_unique< SpectrumAnalyzer >(sampleRate,
                                                                  postEqBufferL,
                                                                  postEqBufferR,
                                                                  apvts,
                                                                  ColourPalette::AnalyzerPostEq.withAlpha(0.25f));
    responseCurve          = std::make_unique< ResponseCurve >(apvts, sampleRate);

    addAndMakeVisible(*backgroundGrid);
    addAndMakeVisible(*spectrumAnalyzerPreEq);
    addAndMakeVisible(*spectrumAnalyzerPostEq);
    addAndMakeVisible(*responseCurve);
}

/*---------------------------------------------------------------------------
**
*/
void
AnalyzerContainer::resized()
{
    if (backgroundGrid == nullptr || spectrumAnalyzerPreEq == nullptr || spectrumAnalyzerPostEq == nullptr
        || responseCurve == nullptr) {
        return;
    }

    auto bounds = getLocalBounds();
    backgroundGrid->setBounds(bounds);
    spectrumAnalyzerPreEq->setBounds(bounds);
    spectrumAnalyzerPostEq->setBounds(bounds);
    responseCurve->setBounds(bounds);
}

/*---------------------------------------------------------------------------
**
*/
size_t
AnalyzerContainer::getActiveNodeIndex()
{
    if (responseCurve != nullptr) {
        return responseCurve->getActiveNodeIndex();
    }

    return 0;
}

/*---------------------------------------------------------------------------
** End of File
*/
