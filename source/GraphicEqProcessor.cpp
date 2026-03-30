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
    , filter_factory_(this)
{
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

    filter_factory_.updateBandEnablements(left_chain_);
    filter_factory_.updateBandEnablements(right_chain_);

    if (getLowCutEnabled()) {
        filter_factory_.updateCutCoefficients(left_chain_, FilterFactory::ChainPositions::LowCut);
        filter_factory_.updateCutCoefficients(right_chain_, FilterFactory::ChainPositions::LowCut);
    }

    if (getHighCutEnabled()) {
        filter_factory_.updateCutCoefficients(left_chain_, FilterFactory::ChainPositions::HighCut);
        filter_factory_.updateCutCoefficients(right_chain_, FilterFactory::ChainPositions::HighCut);
    }

    for (int i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        if (getBandEnabled(i)) {
            const auto chain_pos = static_cast< FilterFactory::ChainPositions >(i + 1);

            filter_factory_.updatePeakCoefficients(left_chain_, chain_pos, i);
            filter_factory_.updatePeakCoefficients(right_chain_, chain_pos, i);
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
    filter_factory_.updateBandEnablements(left_chain_);
    filter_factory_.updateBandEnablements(right_chain_);

    const auto sample_rate = getSampleRate();

    if (getLowCutEnabled()) {
        filter_factory_.updateCutCoefficients(left_chain_, FilterFactory::ChainPositions::LowCut);
        filter_factory_.updateCutCoefficients(right_chain_, FilterFactory::ChainPositions::LowCut);
    }

    if (getHighCutEnabled()) {
        filter_factory_.updateCutCoefficients(left_chain_, FilterFactory::ChainPositions::HighCut);
        filter_factory_.updateCutCoefficients(right_chain_, FilterFactory::ChainPositions::HighCut);
    }

    for (int i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        if (getBandEnabled(i)) {
            const auto chain_pos = static_cast< FilterFactory::ChainPositions >(i + 1);

            filter_factory_.updatePeakCoefficients(left_chain_, chain_pos, i);
            filter_factory_.updatePeakCoefficients(right_chain_, chain_pos, i);
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
FilterFactory&
GraphicEqProcessor::getFilterFactory()
{
    return filter_factory_;
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
    using namespace EqProperties;

    const auto& parameter_name = getPeakControlParamName(PeakControl::ENABLED, band_index);
    const auto* p              = dynamic_cast< juce::AudioParameterBool* >(apvts_.getParameter(parameter_name));

    return (p != nullptr) && p->get();
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getBandFrequency(const int band_index) const
{
    using namespace EqProperties;

    const auto& parameter_name = getPeakControlParamName(PeakControl::FREQUENCY, band_index);
    const auto* p              = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(parameter_name));

    return (p != nullptr) ? p->get() : 0.f;
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getBandGain(const int band_index) const
{
    using namespace EqProperties;

    const auto& parameter_name = getPeakControlParamName(PeakControl::GAIN, band_index);
    const auto* p              = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(parameter_name));

    return (p != nullptr) ? juce::Decibels::decibelsToGain(p->get()) : 0.f;
}

/*---------------------------------------------------------------------------
**
*/
float
GraphicEqProcessor::getBandQ(const int band_index) const
{
    using namespace EqProperties;

    const auto& parameter_name = getPeakControlParamName(PeakControl::QUALITY, band_index);
    const auto* p              = dynamic_cast< juce::AudioParameterFloat* >(apvts_.getParameter(parameter_name));

    return (p != nullptr) ? p->get() : 0.f;
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
