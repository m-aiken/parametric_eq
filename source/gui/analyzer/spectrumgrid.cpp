#include "ColourPalette.h"
#include "GraphicEqGlobals.h"
#include "SpectrumGrid.h"

/*---------------------------------------------------------------------------
**
*/
void
SpectrumGrid::paint(juce::Graphics& g)
{
    const auto& bounds       = getLocalBounds();
    const auto& boundsX      = bounds.getX();
    const auto& boundsY      = bounds.getY();
    const auto& boundsWidth  = bounds.getWidth();
    const auto& boundsBottom = bounds.getBottom();
    const auto& boundsRight  = bounds.getRight();

    g.setFont(GraphicEq::getFont());
    g.setColour(ColourPalette::Text.withAlpha(0.2f));

    // dB markers
    for (auto i = GraphicEq::MIN_DB; i <= GraphicEq::MAX_DB; i += 12) {
        auto yCoord = juce::jmap< float >(i, GraphicEq::MIN_DB, GraphicEq::MAX_DB, boundsBottom, boundsY);
        g.drawLine(boundsX + 2, yCoord, boundsRight - 2, yCoord, 1.f);
    }

    // Frequency markers
    auto inCatchGroup = [](int i) -> bool {
        return (i <= 50 && i % 10 == 0) || (i <= 200 && i % 20 == 0) || (i <= 1000 && i % 100 == 0)
               || (i <= 2000 && i % 200 == 0) || (i <= 5000 && i % 500 == 0) || (i % 1000 == 0);
    };

    for (auto i = GraphicEq::MIN_HZ + 1; i < GraphicEq::MAX_HZ - 1; ++i) {
        if (inCatchGroup(i)) {
            auto alpha = i == 50 || i == 100 || i == 200 || i == 500 || i == 1000 || i == 2000 || i == 5000
                                 || i == 10000 ?
                             0.2f :
                             0.07f;
            g.setColour(ColourPalette::Text.withAlpha(alpha));
            auto normalizedX = juce::mapFromLog10< float >(i, GraphicEq::MIN_HZ, GraphicEq::MAX_HZ);
            auto freqLineX   = boundsX + boundsWidth * normalizedX;
            g.drawVerticalLine(freqLineX, boundsY, boundsBottom);
        }
    }
}

/*---------------------------------------------------------------------------
** End of File
*/
