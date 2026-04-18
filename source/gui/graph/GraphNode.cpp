#include "GraphNode.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphNode::GraphNode(juce::AudioProcessorValueTreeState& apvts, const juce::String& band_enablement_parameter)
    : attachment_(apvts, band_enablement_parameter, *this)
    , coordinates_(0, 0)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

/*---------------------------------------------------------------------------
**
*/
void
GraphNode::paint(juce::Graphics& g)
{
    g.setColour(getToggleState() ? Theme::NODE_ON : Theme::NODE_OFF);
    g.fillEllipse(getLocalBounds().toFloat());
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
** End of File
*/
