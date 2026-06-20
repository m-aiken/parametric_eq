#include "ParametricEqEditor.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
ParametricEqEditor::ParametricEqEditor(ParametricEqProcessor& p)
    : AudioProcessorEditor(&p)
    , processor_ref_(p)
    , graph_(p)
    , eq_controls_(processor_ref_)
    , queued_active_band_(ParametricEq::NULL_BAND)
{
    ParametricEq::addListenerToAllParameters(processor_ref_.getApvts(), this);

    addAndMakeVisible(graph_);
    addAndMakeVisible(eq_controls_);

    setSize(1024, 600);

    startTimerHz(60);
}

/*---------------------------------------------------------------------------
**
*/
ParametricEqEditor::~ParametricEqEditor()
{
    stopTimer();
    ParametricEq::removeListenerFromAllParameters(processor_ref_.getApvts(), this);
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqEditor::paint(juce::Graphics& g)
{
    g.fillAll(ParametricEq::Theme::MAIN_BACKGROUND);
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqEditor::resized()
{
    using Track = juce::Grid::TrackInfo;
    using Fr    = juce::Grid::Fr;

    const auto& bounds = getLocalBounds();
    juce::Grid  grid;

    grid.autoColumns = Track(Fr(100));

    grid.templateRows = {
        Track(Fr(80)),  //! Graph.
        Track(Fr(20)),  //! Band controls.
    };

    grid.items.add(juce::GridItem(graph_));
    grid.items.add(juce::GridItem(eq_controls_));

    grid.performLayout(bounds);
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqEditor::parameterChanged(const juce::String& parameter_id, float new_value)
{
    juce::ignoreUnused(new_value);

    const auto&             apvts       = processor_ref_.getApvts();
    ParametricEq::FILTER_ID active_band = ParametricEq::NULL_BAND;

    for (int i = ParametricEq::LOW_CUT; i <= ParametricEq::HIGH_CUT; ++i) {
        const auto filter_id = static_cast< ParametricEq::FILTER_ID >(i);

        const auto& [enabled_, frequency_, gain_, q_, slope_] = ParametricEq::PARAMETER_MAP.at(filter_id);

        // Enablement button: this band is active if its enablement button was toggled to on/true.
        if (parameter_id == enabled_) {
            const auto* p = dynamic_cast< juce::AudioParameterBool* >(apvts.getParameter(parameter_id));

            if (p != nullptr && p->get()) {
                active_band = filter_id;
                break;
            }
        }

        // All other parameters: this band is active if any of their values have changed.
        if (parameter_id == frequency_ || parameter_id == gain_ || parameter_id == q_ || parameter_id == slope_) {
            active_band = filter_id;
            break;
        }
    }

    // Queue the change for the next timer callback.
    // This prevents having to lock the message thread (with juce::MessageManagerLock)
    // because technically we're not repainting due to a trigger from another thread;
    // we're repainting due to a class member having a non-NULL_BAND value.
    queued_active_band_ = active_band;
}

/*---------------------------------------------------------------------------
**
*/
void
ParametricEqEditor::timerCallback()
{
    if (queued_active_band_ != ParametricEq::NULL_BAND) {
        graph_.setActiveBand(queued_active_band_);
        eq_controls_.setActiveBand(queued_active_band_);

        queued_active_band_ = ParametricEq::NULL_BAND;
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
