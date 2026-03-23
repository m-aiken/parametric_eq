#include "AnalyzerContainer.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
AnalyzerContainer::AnalyzerContainer(juce::AudioProcessorValueTreeState&           apvts,
                                     double                                        sampleRate,
                                     MonoBufferFifo< juce::AudioBuffer< float > >& preEqBufferL,
                                     MonoBufferFifo< juce::AudioBuffer< float > >& preEqBufferR,
                                     MonoBufferFifo< juce::AudioBuffer< float > >& postEqBufferL,
                                     MonoBufferFifo< juce::AudioBuffer< float > >& postEqBufferR)
    : spectrumAnalyzerPreEq(sampleRate, preEqBufferL, preEqBufferR, apvts, Theme::ANALYSER_PRE_EQ)
    , spectrumAnalyzerPostEq(sampleRate, postEqBufferL, postEqBufferR, apvts, Theme::ANALYSER_POST_EQ)
    , responseCurve(apvts, sampleRate)
{
    addAndMakeVisible(backgroundGrid);
    addAndMakeVisible(spectrumAnalyzerPreEq);
    addAndMakeVisible(spectrumAnalyzerPostEq);
    addAndMakeVisible(responseCurve);
}

/*---------------------------------------------------------------------------
**
*/
void
AnalyzerContainer::resized()
{
    auto bounds = getLocalBounds();
    backgroundGrid.setBounds(bounds);
    spectrumAnalyzerPreEq.setBounds(bounds);
    spectrumAnalyzerPostEq.setBounds(bounds);
    responseCurve.setBounds(bounds);
}

/*---------------------------------------------------------------------------
**
*/
size_t
AnalyzerContainer::getActiveNodeIndex()
{
    return responseCurve.getActiveNodeIndex();
}

/*---------------------------------------------------------------------------
** End of File
*/
