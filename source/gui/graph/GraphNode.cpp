#include "GraphNode.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphNode::GraphNode(juce::AudioProcessorValueTreeState& apvts, const juce::String& band_enablement_parameter)
    : attachment_(apvts, band_enablement_parameter, *this)
{
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
** End of File
*/
