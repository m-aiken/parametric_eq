#include "ThemeButton.h"

/*---------------------------------------------------------------------------
**
*/
ThemeButton::ThemeButton()
{
    icon = std::make_unique< DrawableImage >();

    if (icon != nullptr) {
        icon->setImage(juce::ImageFileFormat::loadFrom(BinaryData::day_and_night_icon_png,
                                                       BinaryData::day_and_night_icon_pngSize));
    }
}

/*---------------------------------------------------------------------------
**
*/
void
ThemeButton::paint(juce::Graphics& g)
{
    if (icon != nullptr) {
        icon->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 0.7f);
    }
}

/*---------------------------------------------------------------------------
**
*/
void
ThemeButton::mouseEnter(const juce::MouseEvent& e)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

/*---------------------------------------------------------------------------
** End of File
*/
