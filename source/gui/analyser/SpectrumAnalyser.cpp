#include "SpectrumAnalyser.h"

/*---------------------------------------------------------------------------
**
*/
SpectrumAnalyser::SpectrumAnalyser(const double        sample_rate,
                                   MonoBufferFifo&     left_scsf,
                                   MonoBufferFifo&     right_scsf,
                                   const juce::Colour& colour)
    : colour_(colour)
    , left_path_producer_(sample_rate, left_scsf)
    , right_path_producer_(sample_rate, right_scsf)
{
    left_path_producer_.toggleProcessing(true);
    right_path_producer_.toggleProcessing(true);

    animate();
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyser::paint(juce::Graphics& g)
{
    const auto& bounds = getLocalBounds();

    left_analyser_path_.applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));
    right_analyser_path_.applyTransform(juce::AffineTransform().translation(bounds.getX(), bounds.getY()));

    g.setColour(colour_);
    g.fillPath(left_analyser_path_);
    g.fillPath(right_analyser_path_);
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyser::resized()
{
    const auto& bounds = getLocalBounds();

    left_path_producer_.setFFTRectBounds(bounds.toFloat());
    right_path_producer_.setFFTRectBounds(bounds.toFloat());
}

/*---------------------------------------------------------------------------
**
*/
void
SpectrumAnalyser::timerCallback()
{
    if (left_path_producer_.getNumAvailableForReading() > 0) {
        while (left_path_producer_.pull(left_analyser_path_)) {
        }  // get most recent
    }

    if (right_path_producer_.getNumAvailableForReading() > 0) {
        while (right_path_producer_.pull(right_analyser_path_)) {
        }  // get most recent
    }

    repaint();
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
