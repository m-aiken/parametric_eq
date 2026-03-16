#pragma once

#include <JuceHeader.h>

#include "Fifo.h"
#include "GraphicEqGlobals.h"

struct FftDataGenerator {
    // TODO should this have a constructor to initialise forwardFFT and window?
    void produceFFTDataForRendering(const juce::AudioBuffer< float >& audioData);
    void changeOrder(GraphicEqGlobals::FFTOrder newOrder);
    int  getFFTSize() const { return 1 << order; }
    int  getNumAvailableFFTDataBlocks() const { return fftDataFifo.getNumAvailableForReading(); }
    bool getFFTData(std::vector< float >& data) { return fftDataFifo.pull(data); }

private:
    GraphicEqGlobals::FFTOrder                               order;
    std::vector< float >                                     fftData;
    std::unique_ptr< juce::dsp::FFT >                        forwardFFT;
    std::unique_ptr< juce::dsp::WindowingFunction< float > > window;

    Fifo< std::vector< float >, 20 > fftDataFifo;
};
