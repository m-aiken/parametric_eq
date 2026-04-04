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
    const auto& low_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_CUT);
    apvts_.getParameter(low_cut_parameters.enabled_)->addListener(this);
    apvts_.getParameter(low_cut_parameters.frequency_)->addListener(this);
    apvts_.getParameter(low_cut_parameters.slope_)->addListener(this);

    const auto& high_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_CUT);
    apvts_.getParameter(high_cut_parameters.enabled_)->addListener(this);
    apvts_.getParameter(high_cut_parameters.frequency_)->addListener(this);
    apvts_.getParameter(high_cut_parameters.slope_)->addListener(this);

    const auto& low_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_SHELF);
    apvts_.getParameter(low_shelf_parameters.enabled_)->addListener(this);
    apvts_.getParameter(low_shelf_parameters.frequency_)->addListener(this);
    apvts_.getParameter(low_shelf_parameters.gain_)->addListener(this);

    const auto& high_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_SHELF);
    apvts_.getParameter(high_shelf_parameters.enabled_)->addListener(this);
    apvts_.getParameter(high_shelf_parameters.frequency_)->addListener(this);
    apvts_.getParameter(high_shelf_parameters.gain_)->addListener(this);

    for (int i = GraphicEq::PEAK_BAND_1; i <= GraphicEq::PEAK_BAND_5; ++i) {
        const auto& peak_band_parameters = GraphicEq::PARAMETER_MAP.at(static_cast< GraphicEq::FILTER_ID >(i));

        apvts_.getParameter(peak_band_parameters.enabled_)->addListener(this);
        apvts_.getParameter(peak_band_parameters.frequency_)->addListener(this);
        apvts_.getParameter(peak_band_parameters.gain_)->addListener(this);
        apvts_.getParameter(peak_band_parameters.q_)->addListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
GraphicEqProcessor::~GraphicEqProcessor()
{
    const auto& low_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_CUT);
    apvts_.getParameter(low_cut_parameters.enabled_)->removeListener(this);
    apvts_.getParameter(low_cut_parameters.frequency_)->removeListener(this);
    apvts_.getParameter(low_cut_parameters.slope_)->removeListener(this);

    const auto& high_cut_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_CUT);
    apvts_.getParameter(high_cut_parameters.enabled_)->removeListener(this);
    apvts_.getParameter(high_cut_parameters.frequency_)->removeListener(this);
    apvts_.getParameter(high_cut_parameters.slope_)->removeListener(this);

    const auto& low_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_SHELF);
    apvts_.getParameter(low_shelf_parameters.enabled_)->removeListener(this);
    apvts_.getParameter(low_shelf_parameters.frequency_)->removeListener(this);
    apvts_.getParameter(low_shelf_parameters.gain_)->removeListener(this);

    const auto& high_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_SHELF);
    apvts_.getParameter(high_shelf_parameters.enabled_)->removeListener(this);
    apvts_.getParameter(high_shelf_parameters.frequency_)->removeListener(this);
    apvts_.getParameter(high_shelf_parameters.gain_)->removeListener(this);

    for (int i = GraphicEq::PEAK_BAND_1; i <= GraphicEq::PEAK_BAND_5; ++i) {
        const auto& peak_band_parameters = GraphicEq::PARAMETER_MAP.at(static_cast< GraphicEq::FILTER_ID >(i));

        apvts_.getParameter(peak_band_parameters.enabled_)->removeListener(this);
        apvts_.getParameter(peak_band_parameters.frequency_)->removeListener(this);
        apvts_.getParameter(peak_band_parameters.gain_)->removeListener(this);
        apvts_.getParameter(peak_band_parameters.q_)->removeListener(this);
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
    const juce::dsp::ProcessSpec spec {
        .sampleRate       = sample_rate,
        .maximumBlockSize = static_cast< uint32 >(samples_per_block),
        .numChannels      = static_cast< uint32 >(getTotalNumOutputChannels()),
    };

    filter_chains_.resize(getTotalNumInputChannels());

    for (auto& chain : filter_chains_) {
        chain.prepare(spec);
        updateFilterChain(chain);
    }
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

    for (auto& chain : filter_chains_) {
        updateFilterChain(chain);
    }
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
GraphicEqProcessor::getFilterEnabled(const GraphicEq::FILTER_ID& filter_id) const
{
    bool        ret = false;
    const auto& id  = GraphicEq::PARAMETER_MAP.at(filter_id).enabled_;

    if (id != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        const auto* p = dynamic_cast< juce::AudioParameterBool* >(apvts_.getParameter(id));

        if (p != nullptr) {
            ret = p->get();
        }
    }

    return ret;
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getFilterFrequency(const GraphicEq::FILTER_ID& filter_id) const
{
    float       ret = GraphicEq::MIN_HZ;
    const auto& id  = GraphicEq::PARAMETER_MAP.at(filter_id).frequency_;

    if (id != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        const auto* p = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(id));

        if (p != nullptr) {
            ret = p->get();
        }
    }

    return ret;
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getFilterGain(const GraphicEq::FILTER_ID& filter_id) const
{
    float       ret = 1.f;
    const auto& id  = GraphicEq::PARAMETER_MAP.at(filter_id).gain_;

    if (id != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        const auto* p = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(id));

        if (p != nullptr) {
            ret = juce::Decibels::decibelsToGain(p->get());
        }
    }

    return ret;
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getFilterQ(const GraphicEq::FILTER_ID& filter_id) const
{
    float       ret = 0.f;
    const auto& id  = GraphicEq::PARAMETER_MAP.at(filter_id).q_;

    if (id != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        const auto* p = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(id));

        if (p != nullptr) {
            ret = p->get();
        }
    }

    return ret;
}

/*---------------------------------------------------------------------------
**
*/
int
GraphicEqProcessor::getFilterOrder(const GraphicEq::FILTER_ID& filter_id) const
{
    int         ret = GraphicEq::SLOPE_48_DB_PER_OCTAVE * 2;
    const auto& id  = GraphicEq::PARAMETER_MAP.at(filter_id).slope_;

    if (id != GraphicEq::PARAMETER_NOT_SUPPORTED) {
        if (const auto* p = dynamic_cast< juce::AudioParameterChoice* >(apvts_.getParameter(id)); p != nullptr) {
            ret = (p->getIndex() + 1) * 2;
        }
    }

    return ret;
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::updateFilterChain(GraphicEq::FilterChain& filter_chain) const
{
    // Update the enabled/disabled state of each filter in the chain.
    filter_factory_.updateBandEnablements(filter_chain);

    // Update the coefficients.
    if (getFilterEnabled(GraphicEq::LOW_CUT)) {
        filter_factory_.updateCutCoefficients(filter_chain, GraphicEq::LOW_CUT);
    }

    if (getFilterEnabled(GraphicEq::LOW_SHELF)) {
        filter_factory_.updateLowShelfCoefficients(filter_chain);
    }

    for (int i = GraphicEq::PEAK_BAND_1; i <= GraphicEq::PEAK_BAND_5; ++i) {
        if (const auto& filter_id = static_cast< GraphicEq::FILTER_ID >(i); getFilterEnabled(filter_id)) {
            filter_factory_.updatePeakCoefficients(filter_chain, filter_id);
        }
    }

    if (getFilterEnabled(GraphicEq::HIGH_SHELF)) {
        filter_factory_.updateHighShelfCoefficients(filter_chain);
    }

    if (getFilterEnabled(GraphicEq::HIGH_CUT)) {
        filter_factory_.updateCutCoefficients(filter_chain, GraphicEq::HIGH_CUT);
    }
}

/*---------------------------------------------------------------------------
**
*/
juce::AudioProcessorValueTreeState::ParameterLayout
GraphicEqProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const auto& low_cut_parameters    = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_CUT);
    const auto& high_cut_parameters   = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_CUT);
    const auto& low_shelf_parameters  = GraphicEq::PARAMETER_MAP.at(GraphicEq::LOW_SHELF);
    const auto& high_shelf_parameters = GraphicEq::PARAMETER_MAP.at(GraphicEq::HIGH_SHELF);

    // Low cut parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(low_cut_parameters.enabled_, 1),
                                                            low_cut_parameters.enabled_,
                                                            GraphicEq::LOW_CUT_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(low_cut_parameters.frequency_, 1),
                                                             low_cut_parameters.frequency_,
                                                             GraphicEq::LOW_CUT_FREQ_NORM_RANGE,
                                                             GraphicEq::LOW_CUT_FREQ_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(low_cut_parameters.slope_, 1),
                                                              low_cut_parameters.slope_,
                                                              GraphicEq::CUT_FILTER_SLOPE_CHOICES,
                                                              GraphicEq::LOW_CUT_SLOPE_DEFAULT));

    // High cut parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(high_cut_parameters.enabled_, 1),
                                                            high_cut_parameters.enabled_,
                                                            GraphicEq::HIGH_CUT_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(high_cut_parameters.frequency_, 1),
                                                             high_cut_parameters.frequency_,
                                                             GraphicEq::HIGH_CUT_FREQ_NORM_RANGE,
                                                             GraphicEq::HIGH_CUT_FREQ_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(high_cut_parameters.slope_, 1),
                                                              high_cut_parameters.slope_,
                                                              GraphicEq::CUT_FILTER_SLOPE_CHOICES,
                                                              GraphicEq::HIGH_CUT_SLOPE_DEFAULT));

    // Low shelf parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(low_shelf_parameters.enabled_, 1),
                                                            low_shelf_parameters.enabled_,
                                                            GraphicEq::LOW_SHELF_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(low_shelf_parameters.frequency_, 1),
                                                             low_shelf_parameters.frequency_,
                                                             GraphicEq::LOW_SHELF_FREQ_NORM_RANGE,
                                                             GraphicEq::LOW_SHELF_FREQ_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(low_shelf_parameters.gain_, 1),
                                                             low_shelf_parameters.gain_,
                                                             GraphicEq::SHELF_GAIN_NORMALISABLE_RANGE,
                                                             GraphicEq::SHELF_DB_DEFAULT));

    // High shelf parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(high_shelf_parameters.enabled_, 1),
                                                            high_shelf_parameters.enabled_,
                                                            GraphicEq::HIGH_SHELF_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(high_shelf_parameters.frequency_, 1),
                                                             high_shelf_parameters.frequency_,
                                                             GraphicEq::HIGH_SHELF_FREQ_NORM_RANGE,
                                                             GraphicEq::HIGH_SHELF_FREQ_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(high_shelf_parameters.gain_, 1),
                                                             high_shelf_parameters.gain_,
                                                             GraphicEq::SHELF_GAIN_NORMALISABLE_RANGE,
                                                             GraphicEq::SHELF_DB_DEFAULT));

    // Peak band parameters.
    for (int i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        const auto& filter_id            = static_cast< GraphicEq::FILTER_ID >(GraphicEq::PEAK_BAND_1 + i);
        const auto& peak_band_parameters = GraphicEq::PARAMETER_MAP.at(filter_id);

        const auto& [min_, max_, default_] = GraphicEq::PEAK_BAND_FREQUENCIES.at(i);

        layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(peak_band_parameters.enabled_, 1),
                                                                peak_band_parameters.enabled_,
                                                                GraphicEq::PEAK_ENABLED_DEFAULT));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(peak_band_parameters.frequency_, 1),
                                                                 peak_band_parameters.frequency_,
                                                                 juce::NormalisableRange(min_, max_, 1.f, 1.f),
                                                                 default_));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(peak_band_parameters.gain_, 1),
                                                                 peak_band_parameters.gain_,
                                                                 GraphicEq::PEAK_GAIN_NORMALISABLE_RANGE,
                                                                 GraphicEq::PEAK_DB_DEFAULT));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(peak_band_parameters.q_, 1),
                                                                 peak_band_parameters.q_,
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
