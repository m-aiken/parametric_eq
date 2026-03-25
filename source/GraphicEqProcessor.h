#pragma once

#include <JuceHeader.h>

#include "FilterUtils.h"
#include "GraphicEqGlobals.h"
#include "MonoBufferFifo.h"
#include "PeakBand.h"

class GraphicEqProcessor final : public juce::AudioProcessor
{
public:
    GraphicEqProcessor();
    ~GraphicEqProcessor() override = default;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int                getNumPrograms() override;
    int                getCurrentProgram() override;
    void               setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void               changeProgramName(int index, const juce::String& new_name) override;

    void prepareToPlay(double sample_rate, int samples_per_block) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer< float >& buffer, juce::MidiBuffer& midi_messages) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool                        hasEditor() const override;
    void                        getStateInformation(juce::MemoryBlock& dest_data) override;
    void                        setStateInformation(const void* data, int size_in_bytes) override;

    juce::AudioProcessorValueTreeState& getApvts();

    MonoBufferFifo preEqMonoFifoL { GraphicEq::Channel::Left };
    MonoBufferFifo preEqMonoFifoR { GraphicEq::Channel::Right };
    MonoBufferFifo postEqMonoFifoL { GraphicEq::Channel::Left };
    MonoBufferFifo postEqMonoFifoR { GraphicEq::Channel::Right };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void updateCutFilter(const FilterUtils::ChainPositions& chain_position,
                         juce::AudioParameterFloat*         freq_param,
                         juce::AudioParameterChoice*        slope_param,
                         double                             sample_rate);

    juce::AudioProcessorValueTreeState apvts_;

    FilterUtils::MonoChain left_chain_;
    FilterUtils::MonoChain right_chain_;

    juce::AudioParameterBool*   low_cut_enabled_param_ { nullptr };
    juce::AudioParameterFloat*  low_cut_freq_param_ { nullptr };
    juce::AudioParameterChoice* low_cut_slope_param_ { nullptr };
    juce::AudioParameterBool*   high_cut_enabled_param_ { nullptr };
    juce::AudioParameterFloat*  high_cut_freq_param_ { nullptr };
    juce::AudioParameterChoice* high_cut_slope_param_ { nullptr };

    std::array< PeakBand, GraphicEq::NUM_PEAK_BANDS > peak_bands_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEqProcessor)
};
