#include "GraphicEqEditor.h"
#include "GraphicEqProcessor.h"

/*---------------------------------------------------------------------------
**
*/
GraphicEqProcessor::GraphicEqProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts_(*this, nullptr, "Parameters", createParameterLayout())
    , filter_factory_(this)
{
    apvts_.getParameter(GraphicEq::LOW_CUT_ENABLED.id_)->addListener(this);
    apvts_.getParameter(GraphicEq::LOW_CUT_FREQ.id_)->addListener(this);
    apvts_.getParameter(GraphicEq::LOW_CUT_SLOPE.id_)->addListener(this);

    apvts_.getParameter(GraphicEq::HIGH_CUT_ENABLED.id_)->addListener(this);
    apvts_.getParameter(GraphicEq::HIGH_CUT_FREQ.id_)->addListener(this);
    apvts_.getParameter(GraphicEq::HIGH_CUT_SLOPE.id_)->addListener(this);

    for (int i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        apvts_.getParameter(getPeakParamName(GraphicEq::PEAK_ENABLED, i))->addListener(this);
        apvts_.getParameter(getPeakParamName(GraphicEq::PEAK_FREQ, i))->addListener(this);
        apvts_.getParameter(getPeakParamName(GraphicEq::PEAK_GAIN, i))->addListener(this);
        apvts_.getParameter(getPeakParamName(GraphicEq::PEAK_Q, i))->addListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
GraphicEqProcessor::~GraphicEqProcessor()
{
    apvts_.getParameter(GraphicEq::LOW_CUT_ENABLED.id_)->removeListener(this);
    apvts_.getParameter(GraphicEq::LOW_CUT_FREQ.id_)->removeListener(this);
    apvts_.getParameter(GraphicEq::LOW_CUT_SLOPE.id_)->removeListener(this);

    apvts_.getParameter(GraphicEq::HIGH_CUT_ENABLED.id_)->removeListener(this);
    apvts_.getParameter(GraphicEq::HIGH_CUT_FREQ.id_)->removeListener(this);
    apvts_.getParameter(GraphicEq::HIGH_CUT_SLOPE.id_)->removeListener(this);

    for (int i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        apvts_.getParameter(getPeakParamName(GraphicEq::PEAK_ENABLED, i))->removeListener(this);
        apvts_.getParameter(getPeakParamName(GraphicEq::PEAK_FREQ, i))->removeListener(this);
        apvts_.getParameter(getPeakParamName(GraphicEq::PEAK_GAIN, i))->removeListener(this);
        apvts_.getParameter(getPeakParamName(GraphicEq::PEAK_Q, i))->removeListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
const juce::String
GraphicEqProcessor::getName() const
{
    return JucePlugin_Name;
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::acceptsMidi() const
{
    return false;
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::producesMidi() const
{
    return false;
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::isMidiEffect() const
{
    return false;
}

/*---------------------------------------------------------------------------
**
*/
double
GraphicEqProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

/*---------------------------------------------------------------------------
**
*/
int
GraphicEqProcessor::getNumPrograms()
{
    return 1;
}

/*---------------------------------------------------------------------------
**
*/
int
GraphicEqProcessor::getCurrentProgram()
{
    return 0;
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

/*---------------------------------------------------------------------------
**
*/
const juce::String
GraphicEqProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::changeProgramName(int index, const juce::String& new_name)
{
    juce::ignoreUnused(index, new_name);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::prepareToPlay(double sample_rate, int samples_per_block)
{
    juce::dsp::ProcessSpec spec {
        .sampleRate       = sample_rate,
        .maximumBlockSize = static_cast< uint32 >(samples_per_block),
        .numChannels      = static_cast< uint32 >(getTotalNumOutputChannels()),
    };

    filter_chains_.resize(getTotalNumInputChannels());

    for (auto& chain : filter_chains_) {
        chain.prepare(spec);
    }

    updateFilterChain();
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::releaseResources()
{
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& output_layout  = layouts.getMainOutputChannelSet();
    const bool  is_mono_out    = output_layout == juce::AudioChannelSet::mono();
    const bool  is_stereo_out  = output_layout == juce::AudioChannelSet::stereo();
    const bool  outs_match_ins = output_layout == layouts.getMainInputChannelSet();

    return (is_mono_out || is_stereo_out) && outs_match_ins;
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::processBlock(juce::AudioBuffer< float >& buffer, juce::MidiBuffer& midi_messages)
{
    juce::ignoreUnused(midi_messages);

    juce::ScopedNoDenormals no_denormals;

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // Push the buffer in its dry state to the pre-EQ FFT FIFO.
    pre_eq_fft_fifo_.pushData(buffer);

    // Process the EQ.
    const auto audio_block = juce::dsp::AudioBlock< float >(buffer);

    for (int i = 0; i < getTotalNumInputChannels(); ++i) {
        auto       single_channel_block   = audio_block.getSingleChannelBlock(i);
        const auto single_channel_context = juce::dsp::ProcessContextReplacing< float >(single_channel_block);

        filter_chains_.at(i).process(single_channel_context);
    }

    // Push the buffer in its wet state to the post-EQ FFT FIFO.
    post_eq_fft_fifo_.pushData(buffer);
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::hasEditor() const
{
    return true;
}

/*---------------------------------------------------------------------------
**
*/
juce::AudioProcessorEditor*
GraphicEqProcessor::createEditor()
{
    return new GraphicEqEditor(*this);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::getStateInformation(juce::MemoryBlock& dest_data)
{
    juce::MemoryOutputStream mos(dest_data, true);
    apvts_.state.writeToStream(mos);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::setStateInformation(const void* data, int size_in_bytes)
{
    const auto& tree = juce::ValueTree::readFromData(data, size_in_bytes);

    if (tree.isValid()) {
        apvts_.replaceState(tree);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::parameterValueChanged(int parameter_index, float new_value)
{
    juce::ignoreUnused(parameter_index, new_value);

    updateFilterChain();
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::parameterGestureChanged(int parameter_index, bool gesture_is_starting)
{
    juce::ignoreUnused(parameter_index, gesture_is_starting);

    // Do nothing. Only implemented because it's a pure virtual method of juce::AudioProcessorParameter::Listener.
}

/*---------------------------------------------------------------------------
**
*/
juce::AudioProcessorValueTreeState&
GraphicEqProcessor::getApvts()
{
    return apvts_;
}

/*---------------------------------------------------------------------------
**
*/
FilterFactory&
GraphicEqProcessor::getFilterFactory()
{
    return filter_factory_;
}

/*---------------------------------------------------------------------------
**
*/
FftInputDataFifo&
GraphicEqProcessor::getPreEqFftFifo()
{
    return pre_eq_fft_fifo_;
}

/*---------------------------------------------------------------------------
**
*/
FftInputDataFifo&
GraphicEqProcessor::getPostEqFftFifo()
{
    return post_eq_fft_fifo_;
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::getLowCutEnabled() const
{
    const auto* p = dynamic_cast< juce::AudioParameterBool* >(apvts_.getParameter(GraphicEq::LOW_CUT_ENABLED.id_));

    return (p != nullptr) && p->get();
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getLowCutFrequency() const
{
    const auto* p = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(GraphicEq::LOW_CUT_FREQ.id_));

    return (p != nullptr) ? p->get() : 0.f;
}

/*---------------------------------------------------------------------------
**
*/
int
GraphicEqProcessor::getLowCutOrder() const
{
    const auto* p = dynamic_cast< juce::AudioParameterChoice* >(apvts_.getParameter(GraphicEq::LOW_CUT_SLOPE.id_));

    return (p != nullptr) ? (p->getIndex() + 1) * 2 : 2;
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::getHighCutEnabled() const
{
    const auto* p = dynamic_cast< juce::AudioParameterBool* >(apvts_.getParameter(GraphicEq::HIGH_CUT_ENABLED.id_));

    return (p != nullptr) && p->get();
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getHighCutFrequency() const
{
    const auto* p = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(GraphicEq::HIGH_CUT_FREQ.id_));

    return (p != nullptr) ? p->get() : 0.f;
}

/*---------------------------------------------------------------------------
**
*/
int
GraphicEqProcessor::getHighCutOrder() const
{
    const auto* p = dynamic_cast< juce::AudioParameterChoice* >(apvts_.getParameter(GraphicEq::HIGH_CUT_SLOPE.id_));

    return (p != nullptr) ? (p->getIndex() + 1) * 2 : 2;
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::getBandEnabled(const int band_index) const
{
    const auto& parameter_name = getPeakParamName(GraphicEq::PEAK_ENABLED, band_index);
    const auto* p              = dynamic_cast< juce::AudioParameterBool* >(apvts_.getParameter(parameter_name));

    return (p != nullptr) && p->get();
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getBandFrequency(const int band_index) const
{
    const auto& parameter_name = getPeakParamName(GraphicEq::PEAK_FREQ, band_index);
    const auto* p              = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(parameter_name));

    return (p != nullptr) ? p->get() : 0.f;
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getBandGain(const int band_index) const
{
    const auto& parameter_name = getPeakParamName(GraphicEq::PEAK_GAIN, band_index);
    const auto* p              = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(parameter_name));

    return (p != nullptr) ? juce::Decibels::decibelsToGain(p->get()) : 0.f;
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getBandQ(const int band_index) const
{
    const auto& parameter_name = getPeakParamName(GraphicEq::PEAK_Q, band_index);
    const auto* p              = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(parameter_name));

    return (p != nullptr) ? p->get() : 0.f;
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::updateFilterChain()
{
    for (auto& chain : filter_chains_) {
        // Update the enabled/disabled state of each filter in the chain.
        filter_factory_.updateBandEnablements(chain);

        // Update the coefficients.
        if (getLowCutEnabled()) {
            filter_factory_.updateCutCoefficients(chain, FilterFactory::ChainPositions::LowCut);
        }

        if (getHighCutEnabled()) {
            filter_factory_.updateCutCoefficients(chain, FilterFactory::ChainPositions::HighCut);
        }

        for (int i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
            if (getBandEnabled(i)) {
                filter_factory_.updatePeakCoefficients(chain, static_cast< FilterFactory::ChainPositions >(i + 1), i);
            }
        }
    }
}

/*---------------------------------------------------------------------------
**
*/
juce::AudioProcessorValueTreeState::ParameterLayout
GraphicEqProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(GraphicEq::LOW_CUT_FREQ.id_, 1),
                                                             GraphicEq::LOW_CUT_FREQ.id_,
                                                             GraphicEq::LOW_CUT_FREQ_NORMALISABLE_RANGE,
                                                             GraphicEq::LOW_CUT_FREQ_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(GraphicEq::LOW_CUT_SLOPE.id_, 1),
                                                              GraphicEq::LOW_CUT_SLOPE.id_,
                                                              GraphicEq::CUT_FILTER_SLOPE_CHOICES,
                                                              GraphicEq::LOW_CUT_SLOPE_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(GraphicEq::LOW_CUT_ENABLED.id_, 1),
                                                            GraphicEq::LOW_CUT_ENABLED.id_,
                                                            GraphicEq::LOW_CUT_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(GraphicEq::HIGH_CUT_FREQ.id_, 1),
                                                             GraphicEq::HIGH_CUT_FREQ.id_,
                                                             GraphicEq::HIGH_CUT_FREQ_NORMALISABLE_RANGE,
                                                             GraphicEq::HIGH_CUT_FREQ_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(GraphicEq::HIGH_CUT_SLOPE.id_, 1),
                                                              GraphicEq::HIGH_CUT_SLOPE.id_,
                                                              GraphicEq::CUT_FILTER_SLOPE_CHOICES,
                                                              GraphicEq::HIGH_CUT_SLOPE_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(GraphicEq::HIGH_CUT_ENABLED.id_, 1),
                                                            GraphicEq::HIGH_CUT_ENABLED.id_,
                                                            GraphicEq::HIGH_CUT_ENABLED_DEFAULT));

    for (int i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        const auto& enabled_name = getPeakParamName(GraphicEq::PEAK_ENABLED, i);
        const auto& freq_name    = getPeakParamName(GraphicEq::PEAK_FREQ, i);
        const auto& gain_name    = getPeakParamName(GraphicEq::PEAK_GAIN, i);
        const auto& q_name       = getPeakParamName(GraphicEq::PEAK_Q, i);

        layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(enabled_name, 1),
                                                                enabled_name,
                                                                GraphicEq::PEAK_ENABLED_DEFAULT));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(freq_name, 1),
                                                                 freq_name,
                                                                 GraphicEq::PEAK_FREQ_NORMALISABLE_RANGE,
                                                                 GraphicEq::PEAK_BAND_CENTRE_FREQUENCIES.at(i)));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(gain_name, 1),
                                                                 gain_name,
                                                                 GraphicEq::PEAK_GAIN_NORMALISABLE_RANGE,
                                                                 GraphicEq::PEAK_GAIN_DEFAULT));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(q_name, 1),
                                                                 q_name,
                                                                 GraphicEq::PEAK_Q_NORMALISABLE_RANGE,
                                                                 GraphicEq::PEAK_Q_DEFAULT));
    }

    return layout;
}

/*---------------------------------------------------------------------------
** This creates new instances of the plugin.
*/
juce::AudioProcessor* JUCE_CALLTYPE
createPluginFilter()
{
    return new GraphicEqProcessor();
}

/*---------------------------------------------------------------------------
** End of File
*/
