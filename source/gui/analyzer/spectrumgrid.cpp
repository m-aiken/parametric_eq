#include "ColourPalette.h"
#include "GraphicEqGlobals.h"
#include "SpectrumGrid.h"

/*---------------------------------------------------------------------------
**
*/
void
SpectrumGrid::paint(juce::Graphics& g)
{
    auto        bounds       = getLocalBounds();
    const float minFreq      = GraphicEqGlobals::getMinFrequency();
    const float maxFreq      = GraphicEqGlobals::getMaxFrequency();
    const float minDb        = GraphicEqGlobals::getNegativeInf();
    const float maxDb        = GraphicEqGlobals::getMaxDecibels();
    auto        boundsX      = bounds.getX();
    auto        boundsY      = bounds.getY();
    auto        boundsWidth  = bounds.getWidth();
    auto        boundsBottom = bounds.getBottom();
    auto        boundsRight  = bounds.getRight();

    g.setFont(GraphicEqGlobals::getFont());

    g.setColour(ColourPalette::getColourV2(ColourPalette::Text).withAlpha(0.2f));

    // dB markers
    for (auto i = minDb; i <= maxDb; i += 12) {
        auto yCoord = juce::jmap< float >(i, minDb, maxDb, boundsBottom, boundsY);
        g.drawLine(boundsX + 2, yCoord, boundsRight - 2, yCoord, 1.f);
    }

    // Frequency markers
    auto inCatchGroup = [](int i) -> bool {
        return (i <= 50 && i % 10 == 0) || (i <= 200 && i % 20 == 0) || (i <= 1000 && i % 100 == 0)
               || (i <= 2000 && i % 200 == 0) || (i <= 5000 && i % 500 == 0) || (i % 1000 == 0);
    };

    for (auto i = minFreq + 1; i < maxFreq - 1; ++i) {
        if (inCatchGroup(i)) {
            auto alpha = i == 50 || i == 100 || i == 200 || i == 500 || i == 1000 || i == 2000 || i == 5000
                                 || i == 10000 ?
                             0.2f :
                             0.07f;
            g.setColour(ColourPalette::getColourV2(ColourPalette::Text).withAlpha(alpha));
            auto normalizedX = juce::mapFromLog10< float >(i, minFreq, maxFreq);
            auto freqLineX   = boundsX + boundsWidth * normalizedX;
            g.drawVerticalLine(freqLineX, boundsY, boundsBottom);
        }
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
