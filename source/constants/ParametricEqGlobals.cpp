#include "ParametricEqGlobals.h"

namespace parametric_eq
{

/*---------------------------------------------------------------------------
**
*/
void
addListenerToAllParameters(juce::AudioProcessorValueTreeState&           apvts,
                           juce::AudioProcessorValueTreeState::Listener* listener)
{
    if (listener == nullptr) {
        return;
    }

    apvts.addParameterListener(PLUGIN_BYPASS_PARAMETER, listener);

    for (auto const& [filter_id, parameters] : PARAMETER_MAP) {
        if (parameters.enabled_ != PARAMETER_NOT_SUPPORTED) {
            apvts.addParameterListener(parameters.enabled_, listener);
        }

        if (parameters.frequency_ != PARAMETER_NOT_SUPPORTED) {
            apvts.addParameterListener(parameters.frequency_, listener);
        }

        if (parameters.gain_ != PARAMETER_NOT_SUPPORTED) {
            apvts.addParameterListener(parameters.gain_, listener);
        }

        if (parameters.q_ != PARAMETER_NOT_SUPPORTED) {
            apvts.addParameterListener(parameters.q_, listener);
        }

        if (parameters.slope_ != PARAMETER_NOT_SUPPORTED) {
            apvts.addParameterListener(parameters.slope_, listener);
        }
    }
}

/*---------------------------------------------------------------------------
**
*/
void
removeListenerFromAllParameters(juce::AudioProcessorValueTreeState&           apvts,
                                juce::AudioProcessorValueTreeState::Listener* listener)
{
    if (listener == nullptr) {
        return;
    }

    apvts.removeParameterListener(PLUGIN_BYPASS_PARAMETER, listener);

    for (auto const& [filter_id, parameters] : PARAMETER_MAP) {
        if (parameters.enabled_ != PARAMETER_NOT_SUPPORTED) {
            apvts.removeParameterListener(parameters.enabled_, listener);
        }

        if (parameters.frequency_ != PARAMETER_NOT_SUPPORTED) {
            apvts.removeParameterListener(parameters.frequency_, listener);
        }

        if (parameters.gain_ != PARAMETER_NOT_SUPPORTED) {
            apvts.removeParameterListener(parameters.gain_, listener);
        }

        if (parameters.q_ != PARAMETER_NOT_SUPPORTED) {
            apvts.removeParameterListener(parameters.q_, listener);
        }

        if (parameters.slope_ != PARAMETER_NOT_SUPPORTED) {
            apvts.removeParameterListener(parameters.slope_, listener);
        }
    }
}

}  // namespace parametric_eq

/*---------------------------------------------------------------------------
** End of File
*/
