#include "FilterFactory.h"
#include "ParametricEqProcessor.h"

/*---------------------------------------------------------------------------
**
*/
FilterFactory::FilterFactory(ParametricEqProcessor* processor_ref)
    : processor_ref_(processor_ref)
{
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateCutCoefficients(ParametricEq::FilterChain& filter_chain, const ParametricEq::FILTER_ID& filter_id) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    const bool is_low_cut = filter_id == ParametricEq::LOW_CUT;

    const auto frequency   = processor_ref_->getFilterFrequency(filter_id);
    const auto order       = processor_ref_->getFilterOrder(filter_id);
    const auto sample_rate = processor_ref_->getSampleRate();

    juce::ReferenceCountedArray< juce::dsp::FilterDesign< float >::IIRCoefficients > coefficients;

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

    auto& filter = is_low_cut ? filter_chain.get< ParametricEq::LOW_CUT >() :
                                filter_chain.get< ParametricEq::HIGH_CUT >();

    filter.setBypassed< ParametricEq::SLOPE_12_DB_PER_OCTAVE >(true);
    filter.setBypassed< ParametricEq::SLOPE_24_DB_PER_OCTAVE >(true);
    filter.setBypassed< ParametricEq::SLOPE_36_DB_PER_OCTAVE >(true);
    filter.setBypassed< ParametricEq::SLOPE_48_DB_PER_OCTAVE >(true);

    const int slope_index = (order / 2) - 1;

    // Note, the switch case fall-through is intentional.
    switch (slope_index) {
    case ParametricEq::SLOPE_48_DB_PER_OCTAVE:
        *filter.get< ParametricEq::SLOPE_48_DB_PER_OCTAVE >().coefficients =
            *coefficients[ParametricEq::SLOPE_48_DB_PER_OCTAVE];
        filter.setBypassed< ParametricEq::SLOPE_48_DB_PER_OCTAVE >(false);

    case ParametricEq::SLOPE_36_DB_PER_OCTAVE:
        *filter.get< ParametricEq::SLOPE_36_DB_PER_OCTAVE >().coefficients =
            *coefficients[ParametricEq::SLOPE_36_DB_PER_OCTAVE];
        filter.setBypassed< ParametricEq::SLOPE_36_DB_PER_OCTAVE >(false);

    case ParametricEq::SLOPE_24_DB_PER_OCTAVE:
        *filter.get< ParametricEq::SLOPE_24_DB_PER_OCTAVE >().coefficients =
            *coefficients[ParametricEq::SLOPE_24_DB_PER_OCTAVE];
        filter.setBypassed< ParametricEq::SLOPE_24_DB_PER_OCTAVE >(false);

    case ParametricEq::SLOPE_12_DB_PER_OCTAVE:
        *filter.get< ParametricEq::SLOPE_12_DB_PER_OCTAVE >().coefficients =
            *coefficients[ParametricEq::SLOPE_12_DB_PER_OCTAVE];
        filter.setBypassed< ParametricEq::SLOPE_12_DB_PER_OCTAVE >(false);
        break;

    default:
        break;
    }
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateLowShelfCoefficients(ParametricEq::FilterChain& filter_chain) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    *filter_chain.get< ParametricEq::LOW_SHELF >().coefficients =
        *juce::dsp::IIR::Coefficients< float >::makeLowShelf(processor_ref_->getSampleRate(),
                                                             processor_ref_->getFilterFrequency(ParametricEq::LOW_SHELF),
                                                             ParametricEq::SHELF_Q_DEFAULT,
                                                             processor_ref_->getFilterGain(ParametricEq::LOW_SHELF));
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateHighShelfCoefficients(ParametricEq::FilterChain& filter_chain) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    *filter_chain.get< ParametricEq::HIGH_SHELF >().coefficients = *juce::dsp::IIR::Coefficients< float >::makeHighShelf(
        processor_ref_->getSampleRate(),
        processor_ref_->getFilterFrequency(ParametricEq::HIGH_SHELF),
        ParametricEq::SHELF_Q_DEFAULT,
        processor_ref_->getFilterGain(ParametricEq::HIGH_SHELF));
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updatePeakCoefficients(ParametricEq::FilterChain&     filter_chain,
                                      const ParametricEq::FILTER_ID& filter_id) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    const auto frequency = processor_ref_->getFilterFrequency(filter_id);
    const auto gain      = processor_ref_->getFilterGain(filter_id);
    const auto q         = processor_ref_->getFilterQ(filter_id);

    const auto coefficients =
        juce::dsp::IIR::Coefficients< float >::makePeakFilter(processor_ref_->getSampleRate(), frequency, q, gain);

    switch (filter_id) {
    case ParametricEq::PEAK_BAND_1:
        *filter_chain.get< ParametricEq::PEAK_BAND_1 >().coefficients = *coefficients;
        break;

    case ParametricEq::PEAK_BAND_2:
        *filter_chain.get< ParametricEq::PEAK_BAND_2 >().coefficients = *coefficients;
        break;

    case ParametricEq::PEAK_BAND_3:
        *filter_chain.get< ParametricEq::PEAK_BAND_3 >().coefficients = *coefficients;
        break;

    case ParametricEq::PEAK_BAND_4:
        *filter_chain.get< ParametricEq::PEAK_BAND_4 >().coefficients = *coefficients;
        break;

    case ParametricEq::PEAK_BAND_5:
        *filter_chain.get< ParametricEq::PEAK_BAND_5 >().coefficients = *coefficients;
        break;

    case ParametricEq::LOW_CUT:
    case ParametricEq::HIGH_CUT:
    case ParametricEq::LOW_SHELF:
    case ParametricEq::HIGH_SHELF:
    case ParametricEq::NULL_BAND:
    default:
        break;
    }
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateBandEnablements(ParametricEq::FilterChain& filter_chain) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    filter_chain.setBypassed< ParametricEq::LOW_CUT >(!processor_ref_->getFilterEnabled(ParametricEq::LOW_CUT));
    filter_chain.setBypassed< ParametricEq::LOW_SHELF >(!processor_ref_->getFilterEnabled(ParametricEq::LOW_SHELF));
    filter_chain.setBypassed< ParametricEq::PEAK_BAND_1 >(!processor_ref_->getFilterEnabled(ParametricEq::PEAK_BAND_1));
    filter_chain.setBypassed< ParametricEq::PEAK_BAND_2 >(!processor_ref_->getFilterEnabled(ParametricEq::PEAK_BAND_2));
    filter_chain.setBypassed< ParametricEq::PEAK_BAND_3 >(!processor_ref_->getFilterEnabled(ParametricEq::PEAK_BAND_3));
    filter_chain.setBypassed< ParametricEq::PEAK_BAND_4 >(!processor_ref_->getFilterEnabled(ParametricEq::PEAK_BAND_4));
    filter_chain.setBypassed< ParametricEq::PEAK_BAND_5 >(!processor_ref_->getFilterEnabled(ParametricEq::PEAK_BAND_5));
    filter_chain.setBypassed< ParametricEq::HIGH_SHELF >(!processor_ref_->getFilterEnabled(ParametricEq::HIGH_SHELF));
    filter_chain.setBypassed< ParametricEq::HIGH_CUT >(!processor_ref_->getFilterEnabled(ParametricEq::HIGH_CUT));
}

/*---------------------------------------------------------------------------
** End of File
*/
