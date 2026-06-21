#include "FilterFactory.h"
#include "ParametricEqProcessor.h"

/*---------------------------------------------------------------------------
**
*/
FilterFactory::FilterFactory(ParametricEqProcessor& processor_ref)
    : processor_ref_(processor_ref)
{
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateCutCoefficients(parametric_eq::FilterChain& filters, const parametric_eq::FILTER_ID& filter_id) const
{
    const bool is_low_cut = filter_id == parametric_eq::LOW_CUT;

    const auto frequency   = processor_ref_.getFilterFrequency(filter_id);
    const auto order       = processor_ref_.getFilterOrder(filter_id);
    const auto sample_rate = processor_ref_.getSampleRate();

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

    auto& filter = is_low_cut ? filters.get< parametric_eq::LOW_CUT >() : filters.get< parametric_eq::HIGH_CUT >();

    filter.setBypassed< parametric_eq::SLOPE_12_DBPO >(true);
    filter.setBypassed< parametric_eq::SLOPE_24_DBPO >(true);
    filter.setBypassed< parametric_eq::SLOPE_36_DBPO >(true);
    filter.setBypassed< parametric_eq::SLOPE_48_DBPO >(true);

    const int slope_index = (order / 2) - 1;

    // Note, the switch case fall-through is intentional.
    switch (slope_index) {
    case parametric_eq::SLOPE_48_DBPO:
        *filter.get< parametric_eq::SLOPE_48_DBPO >().coefficients = *coefficients[parametric_eq::SLOPE_48_DBPO];
        filter.setBypassed< parametric_eq::SLOPE_48_DBPO >(false);

    case parametric_eq::SLOPE_36_DBPO:
        *filter.get< parametric_eq::SLOPE_36_DBPO >().coefficients = *coefficients[parametric_eq::SLOPE_36_DBPO];
        filter.setBypassed< parametric_eq::SLOPE_36_DBPO >(false);

    case parametric_eq::SLOPE_24_DBPO:
        *filter.get< parametric_eq::SLOPE_24_DBPO >().coefficients = *coefficients[parametric_eq::SLOPE_24_DBPO];
        filter.setBypassed< parametric_eq::SLOPE_24_DBPO >(false);

    case parametric_eq::SLOPE_12_DBPO:
        *filter.get< parametric_eq::SLOPE_12_DBPO >().coefficients = *coefficients[parametric_eq::SLOPE_12_DBPO];
        filter.setBypassed< parametric_eq::SLOPE_12_DBPO >(false);
        break;

    default:
        break;
    }
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateLowShelfCoefficients(parametric_eq::FilterChain& filters) const
{
    *filters.get< parametric_eq::LOW_SHELF >().coefficients =
        *juce::dsp::IIR::Coefficients< float >::makeLowShelf(processor_ref_.getSampleRate(),
                                                             processor_ref_.getFilterFrequency(parametric_eq::LOW_SHELF),
                                                             parametric_eq::SHELF_Q_DEFAULT,
                                                             processor_ref_.getFilterGain(parametric_eq::LOW_SHELF));
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateHighShelfCoefficients(parametric_eq::FilterChain& filters) const
{
    *filters.get< parametric_eq::HIGH_SHELF >().coefficients = *juce::dsp::IIR::Coefficients< float >::makeHighShelf(
        processor_ref_.getSampleRate(),
        processor_ref_.getFilterFrequency(parametric_eq::HIGH_SHELF),
        parametric_eq::SHELF_Q_DEFAULT,
        processor_ref_.getFilterGain(parametric_eq::HIGH_SHELF));
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updatePeakCoefficients(parametric_eq::FilterChain& filters, const parametric_eq::FILTER_ID& filter_id) const
{
    const auto frequency = processor_ref_.getFilterFrequency(filter_id);
    const auto gain      = processor_ref_.getFilterGain(filter_id);
    const auto q         = processor_ref_.getFilterQ(filter_id);

    const auto coefficients =
        juce::dsp::IIR::Coefficients< float >::makePeakFilter(processor_ref_.getSampleRate(), frequency, q, gain);

    switch (filter_id) {
    case parametric_eq::PEAK_BAND_1:
        *filters.get< parametric_eq::PEAK_BAND_1 >().coefficients = *coefficients;
        break;

    case parametric_eq::PEAK_BAND_2:
        *filters.get< parametric_eq::PEAK_BAND_2 >().coefficients = *coefficients;
        break;

    case parametric_eq::PEAK_BAND_3:
        *filters.get< parametric_eq::PEAK_BAND_3 >().coefficients = *coefficients;
        break;

    case parametric_eq::PEAK_BAND_4:
        *filters.get< parametric_eq::PEAK_BAND_4 >().coefficients = *coefficients;
        break;

    case parametric_eq::PEAK_BAND_5:
        *filters.get< parametric_eq::PEAK_BAND_5 >().coefficients = *coefficients;
        break;

    case parametric_eq::LOW_CUT:
    case parametric_eq::HIGH_CUT:
    case parametric_eq::LOW_SHELF:
    case parametric_eq::HIGH_SHELF:
    case parametric_eq::NULL_BAND:
    default:
        break;
    }
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateBandEnablements(parametric_eq::FilterChain& filters) const
{
    filters.setBypassed< parametric_eq::LOW_CUT >(!processor_ref_.getFilterEnabled(parametric_eq::LOW_CUT));
    filters.setBypassed< parametric_eq::LOW_SHELF >(!processor_ref_.getFilterEnabled(parametric_eq::LOW_SHELF));
    filters.setBypassed< parametric_eq::PEAK_BAND_1 >(!processor_ref_.getFilterEnabled(parametric_eq::PEAK_BAND_1));
    filters.setBypassed< parametric_eq::PEAK_BAND_2 >(!processor_ref_.getFilterEnabled(parametric_eq::PEAK_BAND_2));
    filters.setBypassed< parametric_eq::PEAK_BAND_3 >(!processor_ref_.getFilterEnabled(parametric_eq::PEAK_BAND_3));
    filters.setBypassed< parametric_eq::PEAK_BAND_4 >(!processor_ref_.getFilterEnabled(parametric_eq::PEAK_BAND_4));
    filters.setBypassed< parametric_eq::PEAK_BAND_5 >(!processor_ref_.getFilterEnabled(parametric_eq::PEAK_BAND_5));
    filters.setBypassed< parametric_eq::HIGH_SHELF >(!processor_ref_.getFilterEnabled(parametric_eq::HIGH_SHELF));
    filters.setBypassed< parametric_eq::HIGH_CUT >(!processor_ref_.getFilterEnabled(parametric_eq::HIGH_CUT));
}

/*---------------------------------------------------------------------------
** End of File
*/
