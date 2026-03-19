#include "EqProperties.h"
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
{
    auto assign_bool_param = [&](auto& target, auto& param_name) {
        if (auto* p = dynamic_cast< juce::AudioParameterBool* >(apvts_.getParameter(param_name)); p != nullptr) {
            target = p;
        }
    };

    auto assign_float_param = [&](auto& target, auto& param_name) {
        if (auto* p = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(param_name)); p != nullptr) {
            target = p;
        }
    };

    auto assign_choice_param = [&](auto& target, auto& param_name) {
        if (auto* p = dynamic_cast< juce::AudioParameterChoice* >(apvts_.getParameter(param_name)); p != nullptr) {
            target = p;
        }
    };

    assign_bool_param(low_cut_enabled_param_, GraphicEq::LOW_CUT_ENABLED.id_);
    assign_float_param(low_cut_freq_param_, GraphicEq::LOW_CUT_FREQ.id_);
    assign_choice_param(low_cut_slope_param_, GraphicEq::LOW_CUT_SLOPE.id_);

    assign_bool_param(high_cut_enabled_param_, GraphicEq::HIGH_CUT_ENABLED.id_);
    assign_float_param(high_cut_freq_param_, GraphicEq::HIGH_CUT_FREQ.id_);
    assign_choice_param(high_cut_slope_param_, GraphicEq::HIGH_CUT_SLOPE.id_);

    for (size_t i = 0; i < peak_bands_.size(); ++i) {
        assign_float_param(peak_bands_.at(i).peakFreqParam,
                           EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i));
        assign_float_param(peak_bands_.at(i).peakGainParam,
                           EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i));
        assign_float_param(peak_bands_.at(i).peakQParam,
                           EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i));
        assign_bool_param(peak_bands_.at(i).peakEnabledParam,
                          EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, i));
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need.
    juce::ignoreUnused(sample_rate);

    juce::dsp::ProcessSpec spec {
        .sampleRate       = sample_rate,
        .maximumBlockSize = static_cast< uint32 >(samples_per_block),
        .numChannels      = static_cast< uint32 >(getTotalNumOutputChannels()),
    };

    left_chain_.prepare(spec);
    right_chain_.prepare(spec);

    FilterUtils::updateBandEnablements(left_chain_, low_cut_enabled_param_, high_cut_enabled_param_, peak_bands_);
    FilterUtils::updateBandEnablements(right_chain_, low_cut_enabled_param_, high_cut_enabled_param_, peak_bands_);

    if (low_cut_enabled_param_ != nullptr && low_cut_enabled_param_->get()) {
        updateCutFilter(FilterUtils::ChainPositions::LowCut, low_cut_freq_param_, low_cut_slope_param_, sample_rate);
    }

    if (high_cut_enabled_param_ != nullptr && high_cut_enabled_param_->get()) {
        updateCutFilter(FilterUtils::ChainPositions::HighCut, high_cut_freq_param_, high_cut_slope_param_, sample_rate);
    }

    for (size_t i = 0; i < peak_bands_.size(); ++i) {
        if (peak_bands_.at(i).peakEnabledParam != nullptr && peak_bands_.at(i).peakEnabledParam->get()) {
            FilterUtils::updatePeakCoefficients(left_chain_,
                                                static_cast< FilterUtils::ChainPositions >(i + 1),
                                                peak_bands_.at(i).peakFreqParam,
                                                peak_bands_.at(i).peakQParam,
                                                peak_bands_.at(i).peakGainParam,
                                                sample_rate);

            FilterUtils::updatePeakCoefficients(right_chain_,
                                                static_cast< FilterUtils::ChainPositions >(i + 1),
                                                peak_bands_.at(i).peakFreqParam,
                                                peak_bands_.at(i).peakQParam,
                                                peak_bands_.at(i).peakGainParam,
                                                sample_rate);
        }
    }

    preEqMonoFifoL.prepare(samples_per_block);
    preEqMonoFifoR.prepare(samples_per_block);

    postEqMonoFifoL.prepare(samples_per_block);
    postEqMonoFifoR.prepare(samples_per_block);
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

    // Populate the pre-eq fifo with the buffer in its dry state
    preEqMonoFifoL.update(buffer);
    preEqMonoFifoR.update(buffer);

    // Start EQ processing
    FilterUtils::updateBandEnablements(left_chain_, low_cut_enabled_param_, high_cut_enabled_param_, peak_bands_);
    FilterUtils::updateBandEnablements(right_chain_, low_cut_enabled_param_, high_cut_enabled_param_, peak_bands_);

    if (low_cut_enabled_param_ != nullptr && low_cut_enabled_param_->get()) {
        updateCutFilter(FilterUtils::ChainPositions::LowCut, low_cut_freq_param_, low_cut_slope_param_, getSampleRate());
    }

    if (high_cut_enabled_param_ != nullptr && high_cut_enabled_param_->get()) {
        updateCutFilter(FilterUtils::ChainPositions::HighCut, high_cut_freq_param_, high_cut_slope_param_, getSampleRate());
    }

    for (size_t i = 0; i < peak_bands_.size(); ++i) {
        if (peak_bands_.at(i).peakEnabledParam != nullptr && peak_bands_.at(i).peakEnabledParam->get()) {
            FilterUtils::updatePeakCoefficients(left_chain_,
                                                static_cast< FilterUtils::ChainPositions >(i + 1),
                                                peak_bands_.at(i).peakFreqParam,
                                                peak_bands_.at(i).peakQParam,
                                                peak_bands_.at(i).peakGainParam,
                                                getSampleRate());

            FilterUtils::updatePeakCoefficients(right_chain_,
                                                static_cast< FilterUtils::ChainPositions >(i + 1),
                                                peak_bands_.at(i).peakFreqParam,
                                                peak_bands_.at(i).peakQParam,
                                                peak_bands_.at(i).peakGainParam,
                                                getSampleRate());
        }
    }

    const auto block       = juce::dsp::AudioBlock< float >(buffer);
    auto       left_block  = block.getSingleChannelBlock(GraphicEq::Channel::Left);
    auto       right_block = block.getSingleChannelBlock(GraphicEq::Channel::Right);
    const auto left_ctx    = juce::dsp::ProcessContextReplacing< float >(left_block);
    const auto right_ctx   = juce::dsp::ProcessContextReplacing< float >(right_block);

    left_chain_.process(left_ctx);
    right_chain_.process(right_ctx);

    postEqMonoFifoL.update(buffer);
    postEqMonoFifoR.update(buffer);
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
juce::AudioProcessorValueTreeState&
GraphicEqProcessor::getApvts()
{
    return apvts_;
}

/*---------------------------------------------------------------------------
**
*/
juce::AudioProcessorValueTreeState::ParameterLayout
GraphicEqProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    EqProperties::addCutParams(layout);
    EqProperties::addPeakParams(layout);

    layout.add(
        std::make_unique< juce::AudioParameterChoice >(juce::ParameterID(GraphicEq::ANALYSER_POINTS.id_, 1),
                                                       GraphicEq::ANALYSER_POINTS.id_,
                                                       juce::StringArray(GraphicEq::FTT_ORDERS.at(GraphicEq::order2048),
                                                                         GraphicEq::FTT_ORDERS.at(GraphicEq::order4096),
                                                                         GraphicEq::FTT_ORDERS.at(GraphicEq::order8192)),
                                                       0));

    return layout;
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::updateCutFilter(const FilterUtils::ChainPositions& chain_position,
                                    juce::AudioParameterFloat*         freq_param,
                                    juce::AudioParameterChoice*        slope_param,
                                    double                             sample_rate)
{
    if (freq_param == nullptr || slope_param == nullptr) {
        return;
    }

    FilterUtils::CoefficientsType coefficients;

    if (chain_position == FilterUtils::ChainPositions::LowCut) {
        coefficients = FilterUtils::makeHighPassFilter(freq_param, slope_param, sample_rate);
    }
    else {
        coefficients = FilterUtils::makeLowPassFilter(freq_param, slope_param, sample_rate);
    }

    FilterUtils::updateCutCoefficients(left_chain_, chain_position, coefficients, slope_param);
    FilterUtils::updateCutCoefficients(right_chain_, chain_position, coefficients, slope_param);
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
