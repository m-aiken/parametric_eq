#pragma once

#include <JuceHeader.h>

#include "FftInputDataFifo.h"
#include "FilterFactory.h"

class GraphicEqProcessor final
    : public juce::AudioProcessor
    , public juce::AudioProcessorParameter::Listener
{
public:
    GraphicEqProcessor();
    ~GraphicEqProcessor() override;

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

    void parameterValueChanged(int parameter_index, float new_value) override;
    void parameterGestureChanged(int parameter_index, bool gesture_is_starting) override;

    juce::AudioProcessorValueTreeState& getApvts();
    FilterFactory&                      getFilterFactory();

    FftInputDataFifo& getPreEqFftFifo();
    FftInputDataFifo& getPostEqFftFifo();

    bool  getLowCutEnabled() const;
    float getLowCutFrequency() const;
    int   getLowCutOrder() const;

    bool  getHighCutEnabled() const;
    float getHighCutFrequency() const;
    int   getHighCutOrder() const;

    bool  getBandEnabled(const int band_index) const;
    float getBandFrequency(const int band_index) const;
    float getBandGain(const int band_index) const;
    float getBandQ(const int band_index) const;

private:
    void updateFilterChain();

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts_;
    FilterFactory                      filter_factory_;

    // vector of filter chains; one per channel.
    std::vector< FilterFactory::FilterChain > filter_chains_;

    FftInputDataFifo pre_eq_fft_fifo_;
    FftInputDataFifo post_eq_fft_fifo_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEqProcessor)
};
