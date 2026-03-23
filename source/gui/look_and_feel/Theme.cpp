#include "Theme.h"

/*---------------------------------------------------------------------------
**
*/
/*static*/ const juce::Colour Theme::DARK_GREY = { 37u, 33u, 38u };
/*static*/ const juce::Colour Theme::PARCHMENT = { 250u, 244u, 237u };
/*static*/ const juce::Colour Theme::RED       = { 214u, 132u, 129u };
/*static*/ const juce::Colour Theme::TEAL      = { 96u, 153u, 162u };
/*static*/ const juce::Colour Theme::PURPLE    = { 103u, 96u, 162u };
/*static*/ const juce::Colour Theme::GREEN     = { 96u, 162u, 107u };

/*---------------------------------------------------------------------------
**
*/
/*static*/ const juce::Colour Theme::MAIN_BACKGROUND      = PARCHMENT;
/*static*/ const juce::Colour Theme::TEXT                 = DARK_GREY;
/*static*/ const juce::Colour Theme::ROTARY_FILL          = RED;
/*static*/ const juce::Colour Theme::ROTARY_THUMB         = DARK_GREY;
/*static*/ const juce::Colour Theme::RESPONSE_CURVE       = RED;
/*static*/ const juce::Colour Theme::RESPONSE_CURVE_AVG   = DARK_GREY;
/*static*/ const juce::Colour Theme::NODE_ON              = RED;
/*static*/ const juce::Colour Theme::NODE_OFF             = DARK_GREY;
/*static*/ const juce::Colour Theme::ANALYSER_GRID        = DARK_GREY;
/*static*/ const juce::Colour Theme::ANALYSER_PRE_EQ      = RED.withAlpha(0.05f);
/*static*/ const juce::Colour Theme::ANALYSER_POST_EQ     = TEAL.withAlpha(0.5f);
/*static*/ const juce::Colour Theme::PEAK_BAND_FOCUS      = RED.withAlpha(0.15f);
/*static*/ const juce::Colour Theme::TEXT_BUTTON_BG_STD   = DARK_GREY.withAlpha(0.01f);
/*static*/ const juce::Colour Theme::TEXT_BUTTON_BG_HOVER = DARK_GREY.withAlpha(0.03f);

/*---------------------------------------------------------------------------
**
*/
/*static*/ juce::Font
Theme::getFont()
{
    return juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 10.f, juce::Font::plain);
}

/*---------------------------------------------------------------------------
** End of File
*/
