#include "GraphNode.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphNode::GraphNode(ParametricEqProcessor& processor_ref, const juce::String& band_enablement_parameter)
    : processor_ref_(processor_ref)
    , attachment_(processor_ref.getApvts(), band_enablement_parameter, *this)
    , coordinates_(0, 0)
    , is_active_band_(false)
{
    processor_ref_.getApvts().addParameterListener(parametric_eq::PLUGIN_BYPASS_PARAMETER, this);
    setEnablement();
}

/*---------------------------------------------------------------------------
**
*/
GraphNode::~GraphNode()
{
    processor_ref_.getApvts().removeParameterListener(parametric_eq::PLUGIN_BYPASS_PARAMETER, this);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphNode::paint(juce::Graphics& g)
{
    const auto& bounds         = getLocalBounds();
    const bool  plugin_enabled = processor_ref_.pluginIsEnabled();
    const bool  band_enabled   = getToggleState();

    if (plugin_enabled) {
        g.setColour(band_enabled ? parametric_eq::theme::NODE_ON : parametric_eq::theme::NODE_OFF);
    }
    else {
        g.setColour(parametric_eq::theme::PLUGIN_DISABLED_NODE);
    }

    // Always draw the centre circle.
    g.fillEllipse(bounds.withSizeKeepingCentre(CENTRE_DIAMETER, CENTRE_DIAMETER).toFloat());

    // If this is the active band, and it's enabled draw an outer ring.
    if (plugin_enabled && band_enabled && is_active_band_) {
        const auto ring_w = bounds.getWidth() - (OUTER_RING_THICKNESS * 2);
        const auto ring_h = bounds.getHeight() - (OUTER_RING_THICKNESS * 2);

        g.drawEllipse(bounds.withSizeKeepingCentre(ring_w, ring_h).toFloat(), OUTER_RING_THICKNESS);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
GraphNode::parameterChanged(const juce::String& parameter_id, float new_value)
{
    juce::ignoreUnused(new_value);

    if (parameter_id == parametric_eq::PLUGIN_BYPASS_PARAMETER) {
        setEnablement();
        repaint();
    }
}

/*---------------------------------------------------------------------------
**
*/
juce::Point< int >&
GraphNode::coordinates()
{
    return coordinates_;
}

/*---------------------------------------------------------------------------
**
*/
void
GraphNode::setIsActiveBand(const bool is_active)
{
    is_active_band_ = is_active;

    repaint();
}

/*---------------------------------------------------------------------------
**
*/
void
GraphNode::setEnablement()
{
    const bool plugin_enabled = processor_ref_.pluginIsEnabled();

    setMouseCursor(plugin_enabled ? juce::MouseCursor::PointingHandCursor : juce::MouseCursor::NormalCursor);
    setEnabled(plugin_enabled);
}

/*---------------------------------------------------------------------------
** End of File
*/
