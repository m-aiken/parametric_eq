#include "GraphWaveform.h"

/*---------------------------------------------------------------------------
**
*/
GraphWaveform::GraphWaveform(const double sample_rate, FftInputDataFifo& fft_input_data_fifo, const juce::Colour& colour)
    : colour_(colour)
    , path_producer_(sample_rate, fft_input_data_fifo)
{
    path_producer_.setProcessingEnabled(true);

    startTimerHz(60);
}

/*---------------------------------------------------------------------------
**
*/
GraphWaveform::~GraphWaveform()
{
    stopTimer();
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::paint(juce::Graphics& g)
{
    const auto& bounds = getLocalBounds().toFloat();

    g.setColour(colour_);

    path_.applyTransform(juce::AffineTransform::translation(bounds.getX(), bounds.getY()));
    g.fillPath(path_);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::resized()
{
    path_producer_.setPathBounds(getLocalBounds().toFloat());
}

/*---------------------------------------------------------------------------
**
*/
void
GraphWaveform::timerCallback()
{
    if (path_producer_.getNumAvailableForReading() > 0) {
        while (path_producer_.pull(path_)) {
        }  // get most recent
    }

    repaint();
}

/*---------------------------------------------------------------------------
** End of File
*/
