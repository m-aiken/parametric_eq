#include "EqProperties.h"
#include "GraphNode.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphNode::GraphNode(juce::AudioProcessorValueTreeState& apvts, const int band_num)
    : attachment_(apvts, EqProperties::getPeakControlParamName(EqProperties::PeakControl::ENABLED, band_num), *this)
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
