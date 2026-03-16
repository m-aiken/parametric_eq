#include "AnalyzerProperties.h"
#include "EqProperties.h"
#include "GraphicEqEditor.h"
#include "GraphicEqProcessor.h"

/*---------------------------------------------------------------------------
**
*/
GraphicEqProcessor::GraphicEqProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
    #if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
    #endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
{
    auto assignBoolParam = [&](auto& target, auto& paramName) {
        auto param = dynamic_cast< juce::AudioParameterBool* >(apvts.getParameter(paramName));
        jassert(param != nullptr);
        target = param;
    };

    auto assignFloatParam = [&](auto& target, auto& paramName) {
        auto param = dynamic_cast< juce::AudioParameterFloat* >(apvts.getParameter(paramName));
        jassert(param != nullptr);
        target = param;
    };

    auto assignChoiceParam = [&](auto& target, auto& paramName) {
        auto param = dynamic_cast< juce::AudioParameterChoice* >(apvts.getParameter(paramName));
        jassert(param != nullptr);
        target = param;
    };

    const auto& eqParams = EqProperties::getCutParams();
    assignBoolParam(lowCutEnabledParam, eqParams.at(EqProperties::CutControls::LOW_CUT_ENABLED));
    assignFloatParam(lowCutFreqParam, eqParams.at(EqProperties::CutControls::LOW_CUT_FREQ));
    assignChoiceParam(lowCutSlopeParam, eqParams.at(EqProperties::CutControls::LOW_CUT_SLOPE));

    assignBoolParam(highCutEnabledParam, eqParams.at(EqProperties::CutControls::HIGH_CUT_ENABLED));
    assignFloatParam(highCutFreqParam, eqParams.at(EqProperties::CutControls::HIGH_CUT_FREQ));
    assignChoiceParam(highCutSlopeParam, eqParams.at(EqProperties::CutControls::HIGH_CUT_SLOPE));

    for (size_t i = 0; i < peakBands.size(); ++i) {
        assignFloatParam(peakBands.at(i).peakFreqParam,
                         EqProperties::getPeakControlParamName(EqProperties::PeakControl::FREQUENCY, i));
        assignFloatParam(peakBands.at(i).peakGainParam,
                         EqProperties::getPeakControlParamName(EqProperties::PeakControl::GAIN, i));
        assignFloatParam(peakBands.at(i).peakQParam,
                         EqProperties::getPeakControlParamName(EqProperties::PeakControl::QUALITY, i));
        assignBoolParam(peakBands.at(i).peakEnabledParam,
                        EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, i));
    }
}

/*---------------------------------------------------------------------------
**
*/
GraphicEqProcessor::~GraphicEqProcessor()
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
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
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
    return 1;  // NB: some hosts don't cope very well if you tell them there are 0 programs,
               // so this should be at least 1, even if you're not really implementing programs.
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
GraphicEqProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need.
    juce::ignoreUnused(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels      = getTotalNumOutputChannels();

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    FilterUtils::updateBandEnablements(leftChain, lowCutEnabledParam, highCutEnabledParam, peakBands);
    FilterUtils::updateBandEnablements(rightChain, lowCutEnabledParam, highCutEnabledParam, peakBands);

    if (lowCutEnabledParam != nullptr && lowCutEnabledParam->get()) {
        updateCutFilter(FilterUtils::ChainPositions::LowCut, lowCutFreqParam, lowCutSlopeParam, sampleRate);
    }

    if (highCutEnabledParam != nullptr && highCutEnabledParam->get()) {
        updateCutFilter(FilterUtils::ChainPositions::HighCut, highCutFreqParam, highCutSlopeParam, sampleRate);
    }

    for (size_t i = 0; i < peakBands.size(); ++i) {
        if (peakBands.at(i).peakEnabledParam != nullptr && peakBands.at(i).peakEnabledParam->get()) {
            FilterUtils::updatePeakCoefficients(leftChain,
                                                static_cast< FilterUtils::ChainPositions >(i + 1),
                                                peakBands.at(i).peakFreqParam,
                                                peakBands.at(i).peakQParam,
                                                peakBands.at(i).peakGainParam,
                                                sampleRate);

            FilterUtils::updatePeakCoefficients(rightChain,
                                                static_cast< FilterUtils::ChainPositions >(i + 1),
                                                peakBands.at(i).peakFreqParam,
                                                peakBands.at(i).peakQParam,
                                                peakBands.at(i).peakGainParam,
                                                sampleRate);
        }
    }

    preEqMonoFifoL.prepare(samplesPerBlock);
    preEqMonoFifoR.prepare(samplesPerBlock);

    postEqMonoFifoL.prepare(samplesPerBlock);
    postEqMonoFifoR.prepare(samplesPerBlock);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
#endif
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::processBlock(juce::AudioBuffer< float >& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto                    totalNumInputChannels  = getTotalNumInputChannels();
    auto                    totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Populate the pre-eq fifo with the buffer in its dry state
    preEqMonoFifoL.update(buffer);
    preEqMonoFifoR.update(buffer);

    // Start EQ processing
    FilterUtils::updateBandEnablements(leftChain, lowCutEnabledParam, highCutEnabledParam, peakBands);
    FilterUtils::updateBandEnablements(rightChain, lowCutEnabledParam, highCutEnabledParam, peakBands);

    if (lowCutEnabledParam != nullptr && lowCutEnabledParam->get()) {
        updateCutFilter(FilterUtils::ChainPositions::LowCut, lowCutFreqParam, lowCutSlopeParam, getSampleRate());
    }

    if (highCutEnabledParam != nullptr && highCutEnabledParam->get()) {
        updateCutFilter(FilterUtils::ChainPositions::HighCut, highCutFreqParam, highCutSlopeParam, getSampleRate());
    }

    for (size_t i = 0; i < peakBands.size(); ++i) {
        if (peakBands.at(i).peakEnabledParam != nullptr && peakBands.at(i).peakEnabledParam->get()) {
            FilterUtils::updatePeakCoefficients(leftChain,
                                                static_cast< FilterUtils::ChainPositions >(i + 1),
                                                peakBands.at(i).peakFreqParam,
                                                peakBands.at(i).peakQParam,
                                                peakBands.at(i).peakGainParam,
                                                getSampleRate());

            FilterUtils::updatePeakCoefficients(rightChain,
                                                static_cast< FilterUtils::ChainPositions >(i + 1),
                                                peakBands.at(i).peakFreqParam,
                                                peakBands.at(i).peakQParam,
                                                peakBands.at(i).peakGainParam,
                                                getSampleRate());
        }
    }

    juce::dsp::AudioBlock< float > block(buffer);
    auto                           leftBlock  = block.getSingleChannelBlock(GraphicEqGlobals::Channel::Left);
    auto                           rightBlock = block.getSingleChannelBlock(GraphicEqGlobals::Channel::Right);

    juce::dsp::ProcessContextReplacing< float > leftCtx(leftBlock);
    juce::dsp::ProcessContextReplacing< float > rightCtx(rightBlock);

    leftChain.process(leftCtx);
    rightChain.process(rightCtx);

    postEqMonoFifoL.update(buffer);
    postEqMonoFifoR.update(buffer);
}

/*---------------------------------------------------------------------------
**
*/
bool
GraphicEqProcessor::hasEditor() const
{
    return true;  // (change this to false if you choose to not supply an editor)
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
GraphicEqProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphicEqProcessor::updateCutFilter(const FilterUtils::ChainPositions& chainPosition,
                                    juce::AudioParameterFloat*         freqParam,
                                    juce::AudioParameterChoice*        slopeParam,
                                    double                             sampleRate)
{
    if (freqParam == nullptr || slopeParam == nullptr) {
        return;
    }

    FilterUtils::CoefficientsType coefficients;

    if (chainPosition == FilterUtils::ChainPositions::LowCut) {
        coefficients = FilterUtils::makeHighPassFilter(freqParam, slopeParam, sampleRate);
    }
    else {
        coefficients = FilterUtils::makeLowPassFilter(freqParam, slopeParam, sampleRate);
    }

    FilterUtils::updateCutCoefficients(leftChain, chainPosition, coefficients, slopeParam);
    FilterUtils::updateCutCoefficients(rightChain, chainPosition, coefficients, slopeParam);
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
    AnalyzerProperties::addAnalyzerParams(layout);

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
