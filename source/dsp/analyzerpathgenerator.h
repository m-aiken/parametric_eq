#pragma once

#include <JuceHeader.h>

#include "Fifo.h"

struct AnalyzerPathGenerator {
    void
    generatePath(const std::vector< float >& renderData, juce::Rectangle< float > fftBounds, int fftSize, float binWidth);

    int  getNumPathsAvailable() const;
    bool getPath(juce::Path& path);

private:
    Fifo< juce::Path, 20 > pathFifo;
};
