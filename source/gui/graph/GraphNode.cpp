#include "GraphNode.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphNode::GraphNode(juce::AudioProcessorValueTreeState& apvts, const juce::String& band_enablement_parameter)
    : attachment_(apvts, band_enablement_parameter, *this)
    , coordinates_(0, 0)
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
    const auto& bounds = getLocalBounds();

    g.setColour(getToggleState() ? Theme::NODE_ON : Theme::NODE_OFF);

    // Always draw the centre circle.
    g.fillEllipse(bounds.withSizeKeepingCentre(CENTRE_DIAMETER, CENTRE_DIAMETER).toFloat());

    // If this is the active band, draw an outer ring.
    if (is_active_band_) {
        const auto ring_w = bounds.getWidth() - (OUTER_RING_THICKNESS * 2);
        const auto ring_h = bounds.getHeight() - (OUTER_RING_THICKNESS * 2);

        g.drawEllipse(bounds.withSizeKeepingCentre(ring_w, ring_h).toFloat(), OUTER_RING_THICKNESS);
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
** End of File
*/
