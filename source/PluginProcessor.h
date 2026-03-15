#pragma once

#include <JuceHeader.h>

#include "filterutils.h"
#include "monobufferfifo.h"
#include "globals.h"
#include "peakband.h"

//==============================================================================
class GraphicEqProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    GraphicEqProcessor();
    ~GraphicEqProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool                        hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool   acceptsMidi() const override;
    bool   producesMidi() const override;
    bool   isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int                getNumPrograms() override;
    int                getCurrentProgram() override;
    void               setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void               changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void updateCutFilter(const FilterUtils::ChainPositions& chainPosition, juce::AudioParameterFloat* freqParam, juce::AudioParameterChoice* slopeParam, double sampleRate);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState                         apvts { *this, nullptr, "Parameters", createParameterLayout() };

    MonoBufferFifo<juce::AudioBuffer<float>> preEqMonoFifoL { Globals::Channel::Left };
    MonoBufferFifo<juce::AudioBuffer<float>> preEqMonoFifoR { Globals::Channel::Right };

    MonoBufferFifo<juce::AudioBuffer<float>> postEqMonoFifoL { Globals::Channel::Left };
    MonoBufferFifo<juce::AudioBuffer<float>> postEqMonoFifoR { Globals::Channel::Right };

private:
    FilterUtils::MonoChain leftChain;
    FilterUtils::MonoChain rightChain;

    juce::AudioParameterBool*   lowCutEnabledParam { nullptr };
    juce::AudioParameterFloat*  lowCutFreqParam { nullptr };
    juce::AudioParameterChoice* lowCutSlopeParam { nullptr };
    juce::AudioParameterBool*   highCutEnabledParam { nullptr };
    juce::AudioParameterFloat*  highCutFreqParam { nullptr };
    juce::AudioParameterChoice* highCutSlopeParam { nullptr };

    std::array<PeakBand, Globals::getNumPeakBands()> peakBands;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEqProcessor)
};
