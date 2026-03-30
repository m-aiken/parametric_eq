#include "FilterFactory.h"
#include "GraphicEqProcessor.h"

/*---------------------------------------------------------------------------
**
*/
FilterFactory::FilterFactory(GraphicEqProcessor* processor_ref)
    : processor_ref_(processor_ref)
{
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updatePeakCoefficients(MonoChain& mono_chain, const ChainPositions& chain_position, const int band_index) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    const auto  frequency = processor_ref_->getBandFrequency(band_index);
    const float gain      = processor_ref_->getBandGain(band_index);
    const float q         = processor_ref_->getBandQ(band_index);

    const auto coefficients =
        juce::dsp::IIR::Coefficients< float >::makePeakFilter(processor_ref_->getSampleRate(), frequency, q, gain);

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
FilterFactory::updateCutCoefficients(MonoChain& mono_chain, const ChainPositions& chain_position) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    const bool is_low_cut = chain_position == LowCut;

    const auto frequency   = is_low_cut ? processor_ref_->getLowCutFrequency() : processor_ref_->getHighCutFrequency();
    const auto order       = is_low_cut ? processor_ref_->getLowCutOrder() : processor_ref_->getHighCutOrder();
    const auto sample_rate = processor_ref_->getSampleRate();

    CoefficientsType coefficients;

    if (is_low_cut) {
        coefficients = juce::dsp::FilterDesign< float >::designIIRHighpassHighOrderButterworthMethod(frequency,
                                                                                                     sample_rate,
                                                                                                     order);
    }
    else {
        coefficients = juce::dsp::FilterDesign< float >::designIIRLowpassHighOrderButterworthMethod(frequency,
                                                                                                    sample_rate,
                                                                                                    order);
    }

    if (coefficients.isEmpty()) {
        return;
    }

    auto& filter_chain = is_low_cut ? mono_chain.get< LowCut >() : mono_chain.get< HighCut >();

    filter_chain.setBypassed< Slope_12 >(true);
    filter_chain.setBypassed< Slope_24 >(true);
    filter_chain.setBypassed< Slope_36 >(true);
    filter_chain.setBypassed< Slope_48 >(true);

    const int slope_index = (order / 2) - 1;

    // Note, the switch case fall-through is intentional.
    switch (slope_index) {
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
void
FilterFactory::updateBandEnablements(MonoChain& mono_chain) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    mono_chain.setBypassed< LowCut >(!processor_ref_->getLowCutEnabled());
    mono_chain.setBypassed< Peak_0 >(!processor_ref_->getBandEnabled(0));
    mono_chain.setBypassed< Peak_1 >(!processor_ref_->getBandEnabled(1));
    mono_chain.setBypassed< Peak_2 >(!processor_ref_->getBandEnabled(2));
    mono_chain.setBypassed< Peak_3 >(!processor_ref_->getBandEnabled(3));
    mono_chain.setBypassed< Peak_4 >(!processor_ref_->getBandEnabled(4));
    mono_chain.setBypassed< Peak_5 >(!processor_ref_->getBandEnabled(5));
    mono_chain.setBypassed< HighCut >(!processor_ref_->getHighCutEnabled());
}

/*---------------------------------------------------------------------------
** End of File
*/
