#pragma once

#include <JuceHeader.h>

#include "fifo.h"
#include "globals.h"

//==============================================================================
struct AnalyzerPathGenerator
{
    void generatePath(const std::vector<float>& renderData,
                      juce::Rectangle<float>    fftBounds,
                      int                       fftSize,
                      float                     binWidth,
                      float                     negativeInfinity = Globals::getNegativeInf(),
                      float                     maxDb            = Globals::getMaxDecibels());

    int  getNumPathsAvailable() const;
    bool getPath(juce::Path& path);

private:
    Fifo<juce::Path, 20> pathFifo;
};
