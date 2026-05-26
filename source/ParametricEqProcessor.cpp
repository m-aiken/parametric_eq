#include <regex>

#include "ParametricEqEditor.h"
#include "ParametricEqProcessor.h"

/*---------------------------------------------------------------------------
**
*/
ParametricEqProcessor::ParametricEqProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts_(*this, nullptr, "Parameters", createParameterLayout())
    , filter_factory_(this)
{
    const auto& low_cut_parameters = ParametricEq::PARAMETER_MAP.at(ParametricEq::LOW_CUT);
    apvts_.getParameter(low_cut_parameters.enabled_)->addListener(this);
    apvts_.getParameter(low_cut_parameters.frequency_)->addListener(this);
    apvts_.getParameter(low_cut_parameters.slope_)->addListener(this);

    const auto& high_cut_parameters = ParametricEq::PARAMETER_MAP.at(ParametricEq::HIGH_CUT);
    apvts_.getParameter(high_cut_parameters.enabled_)->addListener(this);
    apvts_.getParameter(high_cut_parameters.frequency_)->addListener(this);
    apvts_.getParameter(high_cut_parameters.slope_)->addListener(this);

    const auto& low_shelf_parameters = ParametricEq::PARAMETER_MAP.at(ParametricEq::LOW_SHELF);
    apvts_.getParameter(low_shelf_parameters.enabled_)->addListener(this);
    apvts_.getParameter(low_shelf_parameters.frequency_)->addListener(this);
    apvts_.getParameter(low_shelf_parameters.gain_)->addListener(this);

    const auto& high_shelf_parameters = ParametricEq::PARAMETER_MAP.at(ParametricEq::HIGH_SHELF);
    apvts_.getParameter(high_shelf_parameters.enabled_)->addListener(this);
    apvts_.getParameter(high_shelf_parameters.frequency_)->addListener(this);
    apvts_.getParameter(high_shelf_parameters.gain_)->addListener(this);

    for (int i = ParametricEq::PEAK_BAND_1; i <= ParametricEq::PEAK_BAND_5; ++i) {
        const auto& peak_band_parameters = ParametricEq::PARAMETER_MAP.at(static_cast< ParametricEq::FILTER_ID >(i));

        apvts_.getParameter(peak_band_parameters.enabled_)->addListener(this);
        apvts_.getParameter(peak_band_parameters.frequency_)->addListener(this);
        apvts_.getParameter(peak_band_parameters.gain_)->addListener(this);
        apvts_.getParameter(peak_band_parameters.q_)->addListener(this);
    }
}

/*---------------------------------------------------------------------------
**
*/
ParametricEqProcessor::~ParametricEqProcessor()
{
    const auto& low_cut_parameters = ParametricEq::PARAMETER_MAP.at(ParametricEq::LOW_CUT);
    apvts_.getParameter(low_cut_parameters.enabled_)->removeListener(this);
    apvts_.getParameter(low_cut_parameters.frequency_)->removeListener(this);
    apvts_.getParameter(low_cut_parameters.slope_)->removeListener(this);

    const auto& high_cut_parameters = ParametricEq::PARAMETER_MAP.at(ParametricEq::HIGH_CUT);
    apvts_.getParameter(high_cut_parameters.enabled_)->removeListener(this);
    apvts_.getParameter(high_cut_parameters.frequency_)->removeListener(this);
    apvts_.getParameter(high_cut_parameters.slope_)->removeListener(this);

    const auto& low_shelf_parameters = ParametricEq::PARAMETER_MAP.at(ParametricEq::LOW_SHELF);
    apvts_.getParameter(low_shelf_parameters.enabled_)->removeListener(this);
    apvts_.getParameter(low_shelf_parameters.frequency_)->removeListener(this);
    apvts_.getParameter(low_shelf_parameters.gain_)->removeListener(this);

    const auto& high_shelf_parameters = ParametricEq::PARAMETER_MAP.at(ParametricEq::HIGH_SHELF);
    apvts_.getParameter(high_shelf_parameters.enabled_)->removeListener(this);
    apvts_.getParameter(high_shelf_parameters.frequency_)->removeListener(this);
    apvts_.getParameter(high_shelf_parameters.gain_)->removeListener(this);

    for (int i = ParametricEq::PEAK_BAND_1; i <= ParametricEq::PEAK_BAND_5; ++i) {
        const auto& peak_band_parameters = ParametricEq::PARAMETER_MAP.at(static_cast< ParametricEq::FILTER_ID >(i));

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
ParametricEqProcessor::getName() const
{
    return JucePlugin_Name;
}

/*---------------------------------------------------------------------------
**
*/
bool
ParametricEqProcessor::acceptsMidi() const
{
    return false;
}

/*---------------------------------------------------------------------------
**
*/
bool
ParametricEqProcessor::producesMidi() const
{
    return false;
}

/*---------------------------------------------------------------------------
**
*/
bool
ParametricEqProcessor::isMidiEffect() const
{
    return false;
}

/*---------------------------------------------------------------------------
**
*/
double
ParametricEqProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

/*---------------------------------------------------------------------------
**
*/
int
ParametricEqProcessor::getNumPrograms()
{
    return 1;
}

/*---------------------------------------------------------------------------
**
*/
int
ParametricEqProcessor::getCurrentProgram()
{
    return 0;
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

/*---------------------------------------------------------------------------
**
*/
const juce::String
ParametricEqProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqProcessor::changeProgramName(int index, const juce::String& new_name)
{
    juce::ignoreUnused(index, new_name);
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqProcessor::prepareToPlay(double sample_rate, int samples_per_block)
{
    const juce::dsp::ProcessSpec spec {
        .sampleRate       = sample_rate,
        .maximumBlockSize = static_cast< uint32 >(samples_per_block),
        .numChannels      = static_cast< uint32 >(getTotalNumOutputChannels()),
    };

    filter_chains_.resize(static_cast< size_t >(getTotalNumInputChannels()));

    for (auto& chain : filter_chains_) {
        chain.prepare(spec);
        updateFilterChain(chain);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqProcessor::releaseResources()
{
}

/*---------------------------------------------------------------------------
**
*/
bool
ParametricEqProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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
ParametricEqProcessor::processBlock(juce::AudioBuffer< float >& buffer, juce::MidiBuffer& midi_messages)
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

#ifdef SHOW_WAVEFORM
    // Push the buffer in its dry state to the pre-EQ FFT FIFO.
    pre_eq_fft_fifo_.pushData(buffer);
#endif

    // Process the EQ.
    const auto audio_block = juce::dsp::AudioBlock< float >(buffer);

    for (size_t i = 0; i < static_cast< size_t >(getTotalNumInputChannels()); ++i) {
        auto       single_channel_block   = audio_block.getSingleChannelBlock(i);
        const auto single_channel_context = juce::dsp::ProcessContextReplacing< float >(single_channel_block);

        filter_chains_.at(i).process(single_channel_context);
    }

#ifdef SHOW_WAVEFORM
    // Push the buffer in its wet state to the post-EQ FFT FIFO.
    post_eq_fft_fifo_.pushData(buffer);
#endif
}

/*---------------------------------------------------------------------------
**
*/
bool
ParametricEqProcessor::hasEditor() const
{
    return true;
}

/*---------------------------------------------------------------------------
**
*/
juce::AudioProcessorEditor*
ParametricEqProcessor::createEditor()
{
    return new ParametricEqEditor(*this);
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqProcessor::getStateInformation(juce::MemoryBlock& dest_data)
{
    juce::MemoryOutputStream mos(dest_data, true);
    apvts_.state.writeToStream(mos);
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqProcessor::setStateInformation(const void* data, int size_in_bytes)
{
    const auto& tree = juce::ValueTree::readFromData(data, static_cast< size_t >(size_in_bytes));

    if (tree.isValid()) {
        apvts_.replaceState(tree);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqProcessor::parameterValueChanged(int parameter_index, float new_value)
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
ParametricEqProcessor::parameterGestureChanged(int parameter_index, bool gesture_is_starting)
{
    juce::ignoreUnused(parameter_index, gesture_is_starting);

    // Do nothing. Only implemented because it's a pure virtual method of juce::AudioProcessorParameter::Listener.
}

/*---------------------------------------------------------------------------
**
*/
juce::AudioProcessorValueTreeState&
ParametricEqProcessor::getApvts()
{
    return apvts_;
}

/*---------------------------------------------------------------------------
**
*/
FftInputDataFifo&
ParametricEqProcessor::getPreEqFftFifo()
{
    return pre_eq_fft_fifo_;
}

/*---------------------------------------------------------------------------
**
*/
FftInputDataFifo&
ParametricEqProcessor::getPostEqFftFifo()
{
    return post_eq_fft_fifo_;
}

/*---------------------------------------------------------------------------
**
*/
bool
ParametricEqProcessor::getFilterEnabled(const ParametricEq::FILTER_ID& filter_id) const
{
    bool        ret = false;
    const auto& id  = ParametricEq::PARAMETER_MAP.at(filter_id).enabled_;

    if (id != ParametricEq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::getFilterFrequency(const ParametricEq::FILTER_ID& filter_id) const
{
    float       ret = ParametricEq::MIN_HZ;
    const auto& id  = ParametricEq::PARAMETER_MAP.at(filter_id).frequency_;

    if (id != ParametricEq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::getFilterGain(const ParametricEq::FILTER_ID& filter_id) const
{
    float       ret = 1.f;
    const auto& id  = ParametricEq::PARAMETER_MAP.at(filter_id).gain_;

    if (id != ParametricEq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::getFilterQ(const ParametricEq::FILTER_ID& filter_id) const
{
    float       ret = 0.f;
    const auto& id  = ParametricEq::PARAMETER_MAP.at(filter_id).q_;

    if (id != ParametricEq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::getFilterOrder(const ParametricEq::FILTER_ID& filter_id) const
{
    int         ret = ParametricEq::SLOPE_48_DBPO * 2;
    const auto& id  = ParametricEq::PARAMETER_MAP.at(filter_id).slope_;

    if (id != ParametricEq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::updateFilterChain(ParametricEq::FilterChain& filter_chain) const
{
    // Update the enabled/disabled state of each filter in the chain.
    filter_factory_.updateBandEnablements(filter_chain);

    // Update the coefficients.
    if (getFilterEnabled(ParametricEq::LOW_CUT)) {
        filter_factory_.updateCutCoefficients(filter_chain, ParametricEq::LOW_CUT);
    }

    if (getFilterEnabled(ParametricEq::LOW_SHELF)) {
        filter_factory_.updateLowShelfCoefficients(filter_chain);
    }

    for (int i = ParametricEq::PEAK_BAND_1; i <= ParametricEq::PEAK_BAND_5; ++i) {
        if (const auto& filter_id = static_cast< ParametricEq::FILTER_ID >(i); getFilterEnabled(filter_id)) {
            filter_factory_.updatePeakCoefficients(filter_chain, filter_id);
        }
    }

    if (getFilterEnabled(ParametricEq::HIGH_SHELF)) {
        filter_factory_.updateHighShelfCoefficients(filter_chain);
    }

    if (getFilterEnabled(ParametricEq::HIGH_CUT)) {
        filter_factory_.updateCutCoefficients(filter_chain, ParametricEq::HIGH_CUT);
    }
}

/*---------------------------------------------------------------------------
**
*/
juce::AudioProcessorValueTreeState::ParameterLayout
ParametricEqProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    const auto hz_string_from_value_func = [](const float value, int /*maximum_string_length*/) -> juce::String {
        const auto rounded_hz = std::round(value);

        if (rounded_hz >= 1000.0f) {
            return juce::String(rounded_hz / 1000.0f, 2) + " kHz";
        }

        return juce::String(rounded_hz) + " Hz";
    };

    const auto hz_param_attributes = juce::AudioParameterFloatAttributes().withStringFromValueFunction(
        hz_string_from_value_func);

    const auto db_string_from_value_func = [](const float value, int /*maximum_string_length*/) -> juce::String {
        return (value > 0 ? "+" : "") + juce::String(value, 2) + " dB";
    };

    const auto db_param_attributes = juce::AudioParameterFloatAttributes().withStringFromValueFunction(
        db_string_from_value_func);

    const auto slope_value_from_string_func = [](const juce::String& string) -> int {
        int ret = ParametricEq::CUT_FILTER_SLOPE_DEFAULT;

        // If the user-provided string can be converted to a 2 digit integer find the
        // closest slope option to the value provided.
        if (std::regex_match(string.toStdString(), std::regex("^[0-9]{1,2}$"))) {
            const auto input   = string.getIntValue();
            int        closest = 0xffff;

            for (int i = 0; i < ParametricEq::CUT_FILTER_SLOPE_CHOICES.size(); ++i) {
                const auto slope    = (i + 1) * 12;
                const auto distance = std::abs(input - slope);

                if (distance < closest) {
                    closest = distance;
                    ret     = i;
                }
            }
        }

        return ret;
    };

    const auto slope_param_attributes = juce::AudioParameterChoiceAttributes().withValueFromStringFunction(
        slope_value_from_string_func);

    const auto& low_cut_parameters    = ParametricEq::PARAMETER_MAP.at(ParametricEq::LOW_CUT);
    const auto& high_cut_parameters   = ParametricEq::PARAMETER_MAP.at(ParametricEq::HIGH_CUT);
    const auto& low_shelf_parameters  = ParametricEq::PARAMETER_MAP.at(ParametricEq::LOW_SHELF);
    const auto& high_shelf_parameters = ParametricEq::PARAMETER_MAP.at(ParametricEq::HIGH_SHELF);

    // Low cut parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(low_cut_parameters.enabled_, 1),
                                                            low_cut_parameters.enabled_,
                                                            ParametricEq::LOW_CUT_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(low_cut_parameters.frequency_, 1),
                                                             low_cut_parameters.frequency_,
                                                             ParametricEq::LOW_CUT_FREQ_NORM_RANGE,
                                                             ParametricEq::LOW_CUT_FREQ_DEFAULT,
                                                             hz_param_attributes));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(low_cut_parameters.slope_, 1),
                                                              low_cut_parameters.slope_,
                                                              ParametricEq::CUT_FILTER_SLOPE_CHOICES,
                                                              ParametricEq::CUT_FILTER_SLOPE_DEFAULT,
                                                              slope_param_attributes));

    // Low shelf parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(low_shelf_parameters.enabled_, 1),
                                                            low_shelf_parameters.enabled_,
                                                            ParametricEq::LOW_SHELF_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(low_shelf_parameters.frequency_, 1),
                                                             low_shelf_parameters.frequency_,
                                                             ParametricEq::LOW_SHELF_FREQ_NORM_RANGE,
                                                             ParametricEq::LOW_SHELF_FREQ_DEFAULT,
                                                             hz_param_attributes));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(low_shelf_parameters.gain_, 1),
                                                             low_shelf_parameters.gain_,
                                                             ParametricEq::SHELF_GAIN_NORMALISABLE_RANGE,
                                                             ParametricEq::SHELF_DB_DEFAULT,
                                                             db_param_attributes));

    // Peak band parameters.
    for (int i = 0; i < ParametricEq::NUM_PEAK_BANDS; ++i) {
        const auto& filter_id            = static_cast< ParametricEq::FILTER_ID >(ParametricEq::PEAK_BAND_1 + i);
        const auto& peak_band_parameters = ParametricEq::PARAMETER_MAP.at(filter_id);
        const auto& default_hz           = ParametricEq::PEAK_BAND_DEFAULT_FREQUENCIES.at(static_cast< size_t >(i));

        layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(peak_band_parameters.enabled_, 1),
                                                                peak_band_parameters.enabled_,
                                                                ParametricEq::PEAK_ENABLED_DEFAULT));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(peak_band_parameters.frequency_, 1),
                                                                 peak_band_parameters.frequency_,
                                                                 ParametricEq::PEAK_FREQ_NORMALISABLE_RANGE,
                                                                 default_hz,
                                                                 hz_param_attributes));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(peak_band_parameters.gain_, 1),
                                                                 peak_band_parameters.gain_,
                                                                 ParametricEq::PEAK_GAIN_NORMALISABLE_RANGE,
                                                                 ParametricEq::PEAK_DB_DEFAULT,
                                                                 db_param_attributes));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(peak_band_parameters.q_, 1),
                                                                 peak_band_parameters.q_,
                                                                 ParametricEq::PEAK_Q_NORMALISABLE_RANGE,
                                                                 ParametricEq::PEAK_Q_DEFAULT));
    }

    // High shelf parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(high_shelf_parameters.enabled_, 1),
                                                            high_shelf_parameters.enabled_,
                                                            ParametricEq::HIGH_SHELF_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(high_shelf_parameters.frequency_, 1),
                                                             high_shelf_parameters.frequency_,
                                                             ParametricEq::HIGH_SHELF_FREQ_NORM_RANGE,
                                                             ParametricEq::HIGH_SHELF_FREQ_DEFAULT,
                                                             hz_param_attributes));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(high_shelf_parameters.gain_, 1),
                                                             high_shelf_parameters.gain_,
                                                             ParametricEq::SHELF_GAIN_NORMALISABLE_RANGE,
                                                             ParametricEq::SHELF_DB_DEFAULT,
                                                             db_param_attributes));

    // High cut parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(high_cut_parameters.enabled_, 1),
                                                            high_cut_parameters.enabled_,
                                                            ParametricEq::HIGH_CUT_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(high_cut_parameters.frequency_, 1),
                                                             high_cut_parameters.frequency_,
                                                             ParametricEq::HIGH_CUT_FREQ_NORM_RANGE,
                                                             ParametricEq::HIGH_CUT_FREQ_DEFAULT,
                                                             hz_param_attributes));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(high_cut_parameters.slope_, 1),
                                                              high_cut_parameters.slope_,
                                                              ParametricEq::CUT_FILTER_SLOPE_CHOICES,
                                                              ParametricEq::CUT_FILTER_SLOPE_DEFAULT,
                                                              slope_param_attributes));

    return layout;
}

/*---------------------------------------------------------------------------
** This creates new instances of the plugin.
*/
juce::AudioProcessor* JUCE_CALLTYPE
createPluginFilter()
{
    return new ParametricEqProcessor();
}

/*---------------------------------------------------------------------------
** End of File
*/
