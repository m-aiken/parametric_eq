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
FilterFactory::updateCutCoefficients(GraphicEq::FilterChain& filter_chain, const GraphicEq::FILTER_ID& filter_id) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    const bool is_low_cut = filter_id == GraphicEq::LOW_CUT;

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

    auto& filter = is_low_cut ? filter_chain.get< GraphicEq::LOW_CUT >() : filter_chain.get< GraphicEq::HIGH_CUT >();

    filter.setBypassed< GraphicEq::SLOPE_12_DB_PER_OCTAVE >(true);
    filter.setBypassed< GraphicEq::SLOPE_24_DB_PER_OCTAVE >(true);
    filter.setBypassed< GraphicEq::SLOPE_36_DB_PER_OCTAVE >(true);
    filter.setBypassed< GraphicEq::SLOPE_48_DB_PER_OCTAVE >(true);

    const int slope_index = (order / 2) - 1;

    // Note, the switch case fall-through is intentional.
    switch (slope_index) {
    case GraphicEq::SLOPE_48_DB_PER_OCTAVE:
        *filter.get< GraphicEq::SLOPE_48_DB_PER_OCTAVE >().coefficients = *coefficients[GraphicEq::SLOPE_48_DB_PER_OCTAVE];
        filter.setBypassed< GraphicEq::SLOPE_48_DB_PER_OCTAVE >(false);

    case GraphicEq::SLOPE_36_DB_PER_OCTAVE:
        *filter.get< GraphicEq::SLOPE_36_DB_PER_OCTAVE >().coefficients = *coefficients[GraphicEq::SLOPE_36_DB_PER_OCTAVE];
        filter.setBypassed< GraphicEq::SLOPE_36_DB_PER_OCTAVE >(false);

    case GraphicEq::SLOPE_24_DB_PER_OCTAVE:
        *filter.get< GraphicEq::SLOPE_24_DB_PER_OCTAVE >().coefficients = *coefficients[GraphicEq::SLOPE_24_DB_PER_OCTAVE];
        filter.setBypassed< GraphicEq::SLOPE_24_DB_PER_OCTAVE >(false);

    case GraphicEq::SLOPE_12_DB_PER_OCTAVE:
        *filter.get< GraphicEq::SLOPE_12_DB_PER_OCTAVE >().coefficients = *coefficients[GraphicEq::SLOPE_12_DB_PER_OCTAVE];
        filter.setBypassed< GraphicEq::SLOPE_12_DB_PER_OCTAVE >(false);
        break;

    default:
        break;
    }
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateLowShelfCoefficients(GraphicEq::FilterChain& filter_chain) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    *filter_chain.get< GraphicEq::LOW_SHELF >().coefficients =
        *juce::dsp::IIR::Coefficients< float >::makeLowShelf(processor_ref_->getSampleRate(),
                                                             processor_ref_->getFilterFrequency(GraphicEq::LOW_SHELF),
                                                             GraphicEq::SHELF_Q_DEFAULT,
                                                             processor_ref_->getFilterGain(GraphicEq::LOW_SHELF));
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateHighShelfCoefficients(GraphicEq::FilterChain& filter_chain) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    *filter_chain.get< GraphicEq::HIGH_SHELF >().coefficients =
        *juce::dsp::IIR::Coefficients< float >::makeHighShelf(processor_ref_->getSampleRate(),
                                                              processor_ref_->getFilterFrequency(GraphicEq::HIGH_SHELF),
                                                              GraphicEq::SHELF_Q_DEFAULT,
                                                              processor_ref_->getFilterGain(GraphicEq::HIGH_SHELF));
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updatePeakCoefficients(GraphicEq::FilterChain& filter_chain, const GraphicEq::FILTER_ID& filter_id) const
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
    case GraphicEq::PEAK_BAND_1:
        *filter_chain.get< GraphicEq::PEAK_BAND_1 >().coefficients = *coefficients;
        break;

    case GraphicEq::PEAK_BAND_2:
        *filter_chain.get< GraphicEq::PEAK_BAND_2 >().coefficients = *coefficients;
        break;

    case GraphicEq::PEAK_BAND_3:
        *filter_chain.get< GraphicEq::PEAK_BAND_3 >().coefficients = *coefficients;
        break;

    case GraphicEq::PEAK_BAND_4:
        *filter_chain.get< GraphicEq::PEAK_BAND_4 >().coefficients = *coefficients;
        break;

    case GraphicEq::PEAK_BAND_5:
        *filter_chain.get< GraphicEq::PEAK_BAND_5 >().coefficients = *coefficients;
        break;

    case GraphicEq::LOW_CUT:
    case GraphicEq::HIGH_CUT:
    case GraphicEq::LOW_SHELF:
    case GraphicEq::HIGH_SHELF:
    default:
        break;
    }
}

/*---------------------------------------------------------------------------
**
*/
void
FilterFactory::updateBandEnablements(GraphicEq::FilterChain& filter_chain) const
{
    if (processor_ref_ == nullptr) {
        return;
    }

    filter_chain.setBypassed< GraphicEq::LOW_CUT >(!processor_ref_->getFilterEnabled(GraphicEq::LOW_CUT));
    filter_chain.setBypassed< GraphicEq::LOW_SHELF >(!processor_ref_->getFilterEnabled(GraphicEq::LOW_SHELF));
    filter_chain.setBypassed< GraphicEq::PEAK_BAND_1 >(!processor_ref_->getFilterEnabled(GraphicEq::PEAK_BAND_1));
    filter_chain.setBypassed< GraphicEq::PEAK_BAND_2 >(!processor_ref_->getFilterEnabled(GraphicEq::PEAK_BAND_2));
    filter_chain.setBypassed< GraphicEq::PEAK_BAND_3 >(!processor_ref_->getFilterEnabled(GraphicEq::PEAK_BAND_3));
    filter_chain.setBypassed< GraphicEq::PEAK_BAND_4 >(!processor_ref_->getFilterEnabled(GraphicEq::PEAK_BAND_4));
    filter_chain.setBypassed< GraphicEq::PEAK_BAND_5 >(!processor_ref_->getFilterEnabled(GraphicEq::PEAK_BAND_5));
    filter_chain.setBypassed< GraphicEq::HIGH_SHELF >(!processor_ref_->getFilterEnabled(GraphicEq::HIGH_SHELF));
    filter_chain.setBypassed< GraphicEq::HIGH_CUT >(!processor_ref_->getFilterEnabled(GraphicEq::HIGH_CUT));
}

/*---------------------------------------------------------------------------
** End of File
*/
