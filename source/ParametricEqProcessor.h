#pragma once

#include <JuceHeader.h>

#include "FftInputDataFifo.h"
#include "FilterFactory.h"

class ParametricEqProcessor final
    : public juce::AudioProcessor
    , public juce::AudioProcessorValueTreeState::Listener
{
public:
    ParametricEqProcessor();
    ~ParametricEqProcessor() override;

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

    juce::AudioProcessorParameter* getBypassParameter() const override;

    void parameterChanged(const juce::String& parameter_id, float new_value) override;

    juce::AudioProcessorValueTreeState& getApvts();

    FftInputDataFifo& getFftFifo();

    bool  getFilterEnabled(const parametric_eq::FILTER_ID& filter_id) const;
    float getFilterFrequency(const parametric_eq::FILTER_ID& filter_id) const;
    float getFilterGain(const parametric_eq::FILTER_ID& filter_id) const;
    float getFilterQ(const parametric_eq::FILTER_ID& filter_id) const;
    int   getFilterOrder(const parametric_eq::FILTER_ID& filter_id) const;

    void updateFilterChain(parametric_eq::FilterChain& filter_chain) const;

    bool pluginIsEnabled() const;
    bool showWaveform() const;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts_;
    FilterFactory                      filter_factory_;

    // vector of filter chains; one per channel.
    std::vector< parametric_eq::FilterChain > filter_chains_;

    FftInputDataFifo fft_fifo_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametricEqProcessor)
};
