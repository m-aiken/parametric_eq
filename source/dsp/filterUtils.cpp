#include "FilterUtils.h"

/*---------------------------------------------------------------------------
**
*/
void
FilterUtils::updatePeakCoefficients(MonoChain&            mono_chain,
                                    const ChainPositions& chain_position,
                                    const PeakBand&       peak_band,
                                    const double          sample_rate)
{
    if (peak_band.freq_ == nullptr || peak_band.q_ == nullptr || peak_band.gain_ == nullptr) {
        return;
    }

    const auto hz   = peak_band.freq_->get();
    const auto q    = peak_band.q_->get();
    const auto gain = juce::Decibels::decibelsToGain(peak_band.gain_->get());

    const auto coefficients = juce::dsp::IIR::Coefficients< float >::makePeakFilter(sample_rate, hz, q, gain);

    switch (chain_position) {
    case Peak_0:
        *mono_chain.get< Peak_0 >().coefficients = *coefficients;
        break;

    case Peak_1:
        *mono_chain.get< Peak_1 >().coefficients = *coefficients;
        break;

    case Peak_2:
        *mono_chain.get< Peak_2 >().coefficients = *coefficients;
        break;

    case Peak_3:
        *mono_chain.get< Peak_3 >().coefficients = *coefficients;
        break;

    case Peak_4:
        *mono_chain.get< Peak_4 >().coefficients = *coefficients;
        break;

    case Peak_5:
        *mono_chain.get< Peak_5 >().coefficients = *coefficients;
        break;

    case LowCut:
    case HighCut:
    default:
        break;
    }
}

/*---------------------------------------------------------------------------
**
*/
void
FilterUtils::updateCutCoefficients(MonoChain&                        mono_chain,
                                   const ChainPositions&             chain_position,
                                   const CoefficientsType&           coefficients,
                                   const juce::AudioParameterChoice* slope_param)
{
    if (slope_param == nullptr) {
        return;
    }

    auto& filter_chain = chain_position == LowCut ? mono_chain.get< LowCut >() : mono_chain.get< HighCut >();

    filter_chain.setBypassed< Slope_12 >(true);
    filter_chain.setBypassed< Slope_24 >(true);
    filter_chain.setBypassed< Slope_36 >(true);
    filter_chain.setBypassed< Slope_48 >(true);

    // Note, the switch case fall-through is intentional.
    switch (slope_param->getIndex()) {
    case Slope_48:
        *filter_chain.get< Slope_48 >().coefficients = *coefficients[Slope_48];
        filter_chain.setBypassed< Slope_48 >(false);

    case Slope_36:
        *filter_chain.get< Slope_36 >().coefficients = *coefficients[Slope_36];
        filter_chain.setBypassed< Slope_36 >(false);

    case Slope_24:
        *filter_chain.get< Slope_24 >().coefficients = *coefficients[Slope_24];
        filter_chain.setBypassed< Slope_24 >(false);

    case Slope_12:
        *filter_chain.get< Slope_12 >().coefficients = *coefficients[Slope_12];
        filter_chain.setBypassed< Slope_12 >(false);
        break;

    default:
        break;
    }
}

/*---------------------------------------------------------------------------
**
*/
FilterUtils::CoefficientsType
FilterUtils::makeHighPassFilter(const juce::AudioParameterFloat*  freq_param,
                                const juce::AudioParameterChoice* slope_param,
                                const double                      sample_rate)
{
    if (freq_param != nullptr && slope_param != nullptr) {
        const auto hz    = freq_param->get();
        const auto order = (slope_param->getIndex() + 1) * 2;

        return juce::dsp::FilterDesign< float >::designIIRHighpassHighOrderButterworthMethod(hz, sample_rate, order);
    }

    return CoefficientsType();
}

/*---------------------------------------------------------------------------
**
*/
FilterUtils::CoefficientsType
FilterUtils::makeLowPassFilter(const juce::AudioParameterFloat*  freq_param,
                               const juce::AudioParameterChoice* slope_param,
                               const double                      sample_rate)
{
    if (freq_param != nullptr && slope_param != nullptr) {
        const auto hz    = freq_param->get();
        const auto order = (slope_param->getIndex() + 1) * 2;

        return juce::dsp::FilterDesign< float >::designIIRLowpassHighOrderButterworthMethod(hz, sample_rate, order);
    }

    return CoefficientsType();
}

/*---------------------------------------------------------------------------
**
*/
void
FilterUtils::updateBandEnablements(MonoChain&                                               mono_chain,
                                   const juce::AudioParameterBool*                          lc_enabled_param,
                                   const juce::AudioParameterBool*                          hc_enabled_param,
                                   const std::array< PeakBand, GraphicEq::NUM_PEAK_BANDS >& peak_bands)
{
    if (lc_enabled_param == nullptr || hc_enabled_param == nullptr) {
        return;
    }

    for (size_t i = 0; i < GraphicEq::NUM_PEAK_BANDS; ++i) {
        if (peak_bands.at(i).enabled_ == nullptr) {
            return;
        }
    }

    mono_chain.setBypassed< LowCut >(!lc_enabled_param->get());
    mono_chain.setBypassed< HighCut >(!hc_enabled_param->get());
    mono_chain.setBypassed< Peak_0 >(!peak_bands.at(0).enabled_->get());
    mono_chain.setBypassed< Peak_1 >(!peak_bands.at(1).enabled_->get());
    mono_chain.setBypassed< Peak_2 >(!peak_bands.at(2).enabled_->get());
    mono_chain.setBypassed< Peak_3 >(!peak_bands.at(3).enabled_->get());
    mono_chain.setBypassed< Peak_4 >(!peak_bands.at(4).enabled_->get());
    mono_chain.setBypassed< Peak_5 >(!peak_bands.at(5).enabled_->get());
}

/*---------------------------------------------------------------------------
** End of File
*/
