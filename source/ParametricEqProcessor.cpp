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
    , filter_factory_(*this)
{
    parametric_eq::addListenerToAllParameters(apvts_, this);
}

/*---------------------------------------------------------------------------
**
*/
ParametricEqProcessor::~ParametricEqProcessor()
{
    parametric_eq::removeListenerFromAllParameters(apvts_, this);
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

    // Hosts vary in how they handle the plugin bypass.
    // Some will set the parameter returned by our getBypassParameter() method;
    // others will simply stop calling processBlock.

    // If we know explicitly that we're bypassed we can exit early.
    if (!pluginIsEnabled()) {
        if (showWaveform()) {
            fft_fifo_.clear();
        }

        // Plugin is bypassed.
        return;
    }

    // Else fall back to a timeout heuristic.
    // Bump the listener to reset the timeout starting point.
    audio_callback_listener_.bump();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // Process the EQ.
    const auto audio_block = juce::dsp::AudioBlock< float >(buffer);

    for (size_t i = 0; i < static_cast< size_t >(getTotalNumInputChannels()); ++i) {
        auto       single_channel_block   = audio_block.getSingleChannelBlock(i);
        const auto single_channel_context = juce::dsp::ProcessContextReplacing< float >(single_channel_block);

        filter_chains_.at(i).process(single_channel_context);
    }

    if (showWaveform()) {
        // Push the buffer to the FFT FIFO.
        fft_fifo_.pushData(buffer);
    }
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
juce::AudioProcessorParameter*
ParametricEqProcessor::getBypassParameter() const
{
    return apvts_.getParameter(parametric_eq::PLUGIN_BYPASS_PARAMETER);
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqProcessor::parameterChanged(const juce::String& parameter_id, float new_value)
{
    juce::ignoreUnused(parameter_id, new_value);

    for (auto& chain : filter_chains_) {
        updateFilterChain(chain);
    }
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
ParametricEqProcessor::getFftFifo()
{
    return fft_fifo_;
}

/*---------------------------------------------------------------------------
**
*/
bool
ParametricEqProcessor::getFilterEnabled(const parametric_eq::FILTER_ID& filter_id) const
{
    bool        ret = false;
    const auto& id  = parametric_eq::PARAMETER_MAP.at(filter_id).enabled_;

    if (id != parametric_eq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::getFilterFrequency(const parametric_eq::FILTER_ID& filter_id) const
{
    float       ret = parametric_eq::MIN_HZ;
    const auto& id  = parametric_eq::PARAMETER_MAP.at(filter_id).frequency_;

    if (id != parametric_eq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::getFilterGain(const parametric_eq::FILTER_ID& filter_id) const
{
    float       ret = 1.f;
    const auto& id  = parametric_eq::PARAMETER_MAP.at(filter_id).gain_;

    if (id != parametric_eq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::getFilterQ(const parametric_eq::FILTER_ID& filter_id) const
{
    float       ret = 0.f;
    const auto& id  = parametric_eq::PARAMETER_MAP.at(filter_id).q_;

    if (id != parametric_eq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::getFilterOrder(const parametric_eq::FILTER_ID& filter_id) const
{
    int         ret = parametric_eq::SLOPE_48_DBPO * 2;
    const auto& id  = parametric_eq::PARAMETER_MAP.at(filter_id).slope_;

    if (id != parametric_eq::PARAMETER_NOT_SUPPORTED) {
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
ParametricEqProcessor::updateFilterChain(parametric_eq::FilterChain& filter_chain) const
{
    // Update the enabled/disabled state of each filter in the chain.
    filter_factory_.updateBandEnablements(filter_chain);

    // Update the coefficients.
    if (getFilterEnabled(parametric_eq::LOW_CUT)) {
        filter_factory_.updateCutCoefficients(filter_chain, parametric_eq::LOW_CUT);
    }

    if (getFilterEnabled(parametric_eq::LOW_SHELF)) {
        filter_factory_.updateLowShelfCoefficients(filter_chain);
    }

    for (int i = parametric_eq::PEAK_BAND_1; i <= parametric_eq::PEAK_BAND_5; ++i) {
        if (const auto& filter_id = static_cast< parametric_eq::FILTER_ID >(i); getFilterEnabled(filter_id)) {
            filter_factory_.updatePeakCoefficients(filter_chain, filter_id);
        }
    }

    if (getFilterEnabled(parametric_eq::HIGH_SHELF)) {
        filter_factory_.updateHighShelfCoefficients(filter_chain);
    }

    if (getFilterEnabled(parametric_eq::HIGH_CUT)) {
        filter_factory_.updateCutCoefficients(filter_chain, parametric_eq::HIGH_CUT);
    }
}

/*---------------------------------------------------------------------------
**
*/
bool
ParametricEqProcessor::pluginIsEnabled() const
{
    bool bypassed = parametric_eq::PLUGIN_BYPASS_DEFAULT;

    if (const auto p = dynamic_cast< juce::AudioParameterBool* >(getBypassParameter()); p != nullptr) {
        bypassed = p->get();
    }

    return !bypassed;
}

/*---------------------------------------------------------------------------
**
*/
bool
ParametricEqProcessor::showWaveform() const
{
    const auto id            = parametric_eq::SHOW_WAVEFORM_PARAMETER;
    bool       show_waveform = parametric_eq::SHOW_WAVEFORM_DEFAULT;

    if (const auto p = dynamic_cast< juce::AudioParameterBool* >(apvts_.getParameter(id)); p != nullptr) {
        show_waveform = p->get();
    }

    return show_waveform;
}

/*---------------------------------------------------------------------------
**
*/
const AudioCallbackListener&
ParametricEqProcessor::getAudioCallbackListener() const
{
    return audio_callback_listener_;
}

/*---------------------------------------------------------------------------
**
*/
juce::AudioProcessorValueTreeState::ParameterLayout
ParametricEqProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Plugin bypass parameter.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(parametric_eq::PLUGIN_BYPASS_PARAMETER, 1),
                                                            parametric_eq::PLUGIN_BYPASS_PARAMETER,
                                                            parametric_eq::PLUGIN_BYPASS_DEFAULT));

    // Show waveform parameter.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(parametric_eq::SHOW_WAVEFORM_PARAMETER, 1),
                                                            parametric_eq::SHOW_WAVEFORM_PARAMETER,
                                                            parametric_eq::SHOW_WAVEFORM_DEFAULT));

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
        int ret = parametric_eq::CUT_FILTER_SLOPE_DEFAULT;

        // If the user-provided string can be converted to a 2 digit integer find the
        // closest slope option to the value provided.
        if (std::regex_match(string.toStdString(), std::regex("^[0-9]{1,2}$"))) {
            const auto input   = string.getIntValue();
            int        closest = 0xffff;

            for (int i = 0; i < parametric_eq::CUT_FILTER_SLOPE_CHOICES.size(); ++i) {
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

    const auto& low_cut_parameters    = parametric_eq::PARAMETER_MAP.at(parametric_eq::LOW_CUT);
    const auto& high_cut_parameters   = parametric_eq::PARAMETER_MAP.at(parametric_eq::HIGH_CUT);
    const auto& low_shelf_parameters  = parametric_eq::PARAMETER_MAP.at(parametric_eq::LOW_SHELF);
    const auto& high_shelf_parameters = parametric_eq::PARAMETER_MAP.at(parametric_eq::HIGH_SHELF);

    // Low cut parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(low_cut_parameters.enabled_, 1),
                                                            low_cut_parameters.enabled_,
                                                            parametric_eq::LOW_CUT_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(low_cut_parameters.frequency_, 1),
                                                             low_cut_parameters.frequency_,
                                                             parametric_eq::LOW_CUT_FREQ_NORM_RANGE,
                                                             parametric_eq::LOW_CUT_FREQ_DEFAULT,
                                                             hz_param_attributes));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(low_cut_parameters.slope_, 1),
                                                              low_cut_parameters.slope_,
                                                              parametric_eq::CUT_FILTER_SLOPE_CHOICES,
                                                              parametric_eq::CUT_FILTER_SLOPE_DEFAULT,
                                                              slope_param_attributes));

    // Low shelf parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(low_shelf_parameters.enabled_, 1),
                                                            low_shelf_parameters.enabled_,
                                                            parametric_eq::LOW_SHELF_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(low_shelf_parameters.frequency_, 1),
                                                             low_shelf_parameters.frequency_,
                                                             parametric_eq::LOW_SHELF_FREQ_NORM_RANGE,
                                                             parametric_eq::LOW_SHELF_FREQ_DEFAULT,
                                                             hz_param_attributes));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(low_shelf_parameters.gain_, 1),
                                                             low_shelf_parameters.gain_,
                                                             parametric_eq::SHELF_GAIN_NORMALISABLE_RANGE,
                                                             parametric_eq::SHELF_DB_DEFAULT,
                                                             db_param_attributes));

    // Peak band parameters.
    for (int i = 0; i < parametric_eq::NUM_PEAK_BANDS; ++i) {
        const auto& filter_id            = static_cast< parametric_eq::FILTER_ID >(parametric_eq::PEAK_BAND_1 + i);
        const auto& peak_band_parameters = parametric_eq::PARAMETER_MAP.at(filter_id);
        const auto& default_hz           = parametric_eq::PEAK_BAND_DEFAULT_FREQUENCIES.at(static_cast< size_t >(i));

        layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(peak_band_parameters.enabled_, 1),
                                                                peak_band_parameters.enabled_,
                                                                parametric_eq::PEAK_ENABLED_DEFAULT));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(peak_band_parameters.frequency_, 1),
                                                                 peak_band_parameters.frequency_,
                                                                 parametric_eq::PEAK_FREQ_NORMALISABLE_RANGE,
                                                                 default_hz,
                                                                 hz_param_attributes));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(peak_band_parameters.gain_, 1),
                                                                 peak_band_parameters.gain_,
                                                                 parametric_eq::PEAK_GAIN_NORMALISABLE_RANGE,
                                                                 parametric_eq::PEAK_DB_DEFAULT,
                                                                 db_param_attributes));

        layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(peak_band_parameters.q_, 1),
                                                                 peak_band_parameters.q_,
                                                                 parametric_eq::PEAK_Q_NORMALISABLE_RANGE,
                                                                 parametric_eq::PEAK_Q_DEFAULT));
    }

    // High shelf parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(high_shelf_parameters.enabled_, 1),
                                                            high_shelf_parameters.enabled_,
                                                            parametric_eq::HIGH_SHELF_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(high_shelf_parameters.frequency_, 1),
                                                             high_shelf_parameters.frequency_,
                                                             parametric_eq::HIGH_SHELF_FREQ_NORM_RANGE,
                                                             parametric_eq::HIGH_SHELF_FREQ_DEFAULT,
                                                             hz_param_attributes));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(high_shelf_parameters.gain_, 1),
                                                             high_shelf_parameters.gain_,
                                                             parametric_eq::SHELF_GAIN_NORMALISABLE_RANGE,
                                                             parametric_eq::SHELF_DB_DEFAULT,
                                                             db_param_attributes));

    // High cut parameters.
    layout.add(std::make_unique< juce::AudioParameterBool >(juce::ParameterID(high_cut_parameters.enabled_, 1),
                                                            high_cut_parameters.enabled_,
                                                            parametric_eq::HIGH_CUT_ENABLED_DEFAULT));

    layout.add(std::make_unique< juce::AudioParameterFloat >(juce::ParameterID(high_cut_parameters.frequency_, 1),
                                                             high_cut_parameters.frequency_,
                                                             parametric_eq::HIGH_CUT_FREQ_NORM_RANGE,
                                                             parametric_eq::HIGH_CUT_FREQ_DEFAULT,
                                                             hz_param_attributes));

    layout.add(std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(high_cut_parameters.slope_, 1),
                                                              high_cut_parameters.slope_,
                                                              parametric_eq::CUT_FILTER_SLOPE_CHOICES,
                                                              parametric_eq::CUT_FILTER_SLOPE_DEFAULT,
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
