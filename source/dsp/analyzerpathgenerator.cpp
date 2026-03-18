#include "AnalyzerPathGenerator.h"
#include "GraphicEqGlobals.h"

/*---------------------------------------------------------------------------
**
*/
void
AnalyzerPathGenerator::generatePath(const std::vector< float >& renderData,
                                    juce::Rectangle< float >    fftBounds,
                                    int                         fftSize,
                                    float                       binWidth)
{
    auto numBins = static_cast< int >(fftSize * 0.5);

    auto fftBoundsBottom = fftBounds.getBottom();
    auto fftBoundsY      = fftBounds.getY();
    auto fftBoundsWidth  = fftBounds.getWidth();

    juce::Path path;
    path.preallocateSpace(3 * static_cast< int >(fftBoundsWidth));

    auto yCoord = juce::jmap< float >(renderData[0], GraphicEq::MIN_DB, GraphicEq::MAX_DB, fftBoundsBottom, fftBoundsY);
    if (std::isnan(yCoord) || std::isinf(yCoord)) {
        yCoord = fftBoundsBottom;
    }

    path.startNewSubPath(0, fftBoundsBottom);

    auto lastXCoord = fftBounds.getX();

    const int pathResolution = 2;

    for (auto i = 1; i < numBins + 1; i += pathResolution) {
        yCoord = juce::jmap< float >(renderData[i], GraphicEq::MIN_DB, GraphicEq::MAX_DB, fftBoundsBottom, fftBoundsY);

        if (!std::isnan(yCoord) && !std::isinf(yCoord)) {
            auto binFreq     = i * binWidth;
            auto normalizedX = juce::mapFromLog10(binFreq, GraphicEq::MIN_HZ, GraphicEq::MAX_HZ);
            auto xCoord      = std::floor(normalizedX * fftBoundsWidth);

            if (xCoord - lastXCoord >= pathResolution && xCoord <= fftBoundsWidth) {
                path.lineTo(xCoord, yCoord);
                lastXCoord = xCoord;
            }
        }
    }

    path.lineTo(fftBoundsWidth, fftBoundsBottom);
    path.closeSubPath();

    pathFifo.push(path);
}

/*---------------------------------------------------------------------------
**
*/
int
AnalyzerPathGenerator::getNumPathsAvailable() const
{
    return pathFifo.getNumAvailableForReading();
}

/*---------------------------------------------------------------------------
**
*/
bool
AnalyzerPathGenerator::getPath(juce::Path& path)
{
    return pathFifo.pull(path);
}

/*---------------------------------------------------------------------------
** End of File
*/
