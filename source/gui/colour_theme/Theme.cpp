#include "Theme.h"

const juce::Colour DARK_GREY = { 37u, 33u, 38u };
const juce::Colour PARCHMENT = { 250u, 244u, 237u };
const juce::Colour RED       = { 214u, 132u, 129u };
const juce::Colour TEAL      = { 96u, 153u, 162u };
const juce::Colour PURPLE    = { 103u, 96u, 162u };
const juce::Colour GREEN     = { 96u, 162u, 107u };

/*---------------------------------------------------------------------------
**
*/
namespace ParametricEq::Theme
{

const juce::Colour MAIN_BACKGROUND        = PARCHMENT;
const juce::Colour TEXT                   = DARK_GREY;
const juce::Colour TEXT_DISABLED          = TEXT.withAlpha(DISABLED_ALPHA);
const juce::Colour ANALYSER_TEXT          = DARK_GREY.withAlpha(0.7f);
const juce::Colour ROTARY_FILL            = RED;
const juce::Colour ROTARY_THUMB           = DARK_GREY;
const juce::Colour RESPONSE_CURVE_FILL    = RED.withAlpha(0.2f);
const juce::Colour RESPONSE_CURVE_AVG     = DARK_GREY;
const juce::Colour NODE_ON                = RED;
const juce::Colour NODE_OFF               = DARK_GREY;
const juce::Colour ANALYSER_GRID          = DARK_GREY;
const juce::Colour ANALYSER_PRE_EQ        = RED.withAlpha(0.05f);
const juce::Colour ANALYSER_POST_EQ       = TEAL.withAlpha(0.5f);
const juce::Colour BAND_BACKGROUND        = RED.withAlpha(0.15f);
const juce::Colour ACTIVE_BAND_BACKGROUND = RED.withAlpha(0.5f);
const juce::Colour TEXT_BUTTON_BG_STD     = DARK_GREY.withAlpha(0.05f);
const juce::Colour TEXT_BUTTON_BG_HOVER   = DARK_GREY.withAlpha(0.08f);
const juce::Colour POWER_ICON             = TEXT;
const juce::Colour POWER_ORB_ON           = GREEN;
const juce::Colour POWER_ORB_OFF          = RED;
const juce::Colour WAVEFORM_BUTTON_ON     = TEXT;
const juce::Colour WAVEFORM_BUTTON_OFF    = TEXT_DISABLED;

const juce::Colour PLUGIN_DISABLED_ANALYSER_TEXT       = TEXT_DISABLED;
const juce::Colour PLUGIN_DISABLED_RESPONSE_CURVE_FILL = DARK_GREY.withAlpha(0.05f);
const juce::Colour PLUGIN_DISABLED_RESPONSE_CURVE_AVG  = TEXT_DISABLED;
const juce::Colour PLUGIN_DISABLED_NODE                = TEXT_DISABLED;
const juce::Colour PLUGIN_DISABLED_BAND_BACKGROUND     = DARK_GREY.withAlpha(0.05f);

/*---------------------------------------------------------------------------
**
*/
juce::Font
getFont()
{
    return juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 10.f, juce::Font::plain);
}

}  // namespace ParametricEq::Theme

/*---------------------------------------------------------------------------
** End of File
*/
