#include "GraphNode.h"
#include "ParametricEqTheme.h"

/*---------------------------------------------------------------------------
**
*/
GraphNode::GraphNode(juce::AudioProcessorValueTreeState& apvts, const juce::String& band_enablement_parameter)
    : attachment_(apvts, band_enablement_parameter, *this)
    , coordinates_(0.f, 0.f)
    , is_active_band_(false)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphNode::paint(juce::Graphics& g)
{
    const auto& bounds       = getLocalBounds().toFloat();
    const bool  band_enabled = getToggleState();

    g.setColour(band_enabled ? parametric_eq::theme::NODE_ON : parametric_eq::theme::NODE_OFF);

    // Always draw the centre circle.
    g.fillEllipse(bounds.withSizeKeepingCentre(CENTRE_DIAMETER, CENTRE_DIAMETER));

    // If this is the active band, and it's enabled draw an outer ring.
    if (band_enabled && is_active_band_) {
        const auto ring_w = bounds.getWidth() - (OUTER_RING_THICKNESS * 2.f);
        const auto ring_h = bounds.getHeight() - (OUTER_RING_THICKNESS * 2.f);

        g.drawEllipse(bounds.withSizeKeepingCentre(ring_w, ring_h), OUTER_RING_THICKNESS);
    }
}

/*---------------------------------------------------------------------------
**
*/
juce::Point< float >&
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
** End of File
*/
