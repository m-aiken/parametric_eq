#include "GraphicEqGlobals.h"
#include "GraphNode.h"
#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
GraphNode::GraphNode(juce::AudioProcessorValueTreeState& apvts, const int band_num)
    : attachment_(apvts, getPeakParamName(GraphicEq::PEAK_ENABLED, band_num), *this)
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
