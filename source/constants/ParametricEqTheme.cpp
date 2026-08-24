#include "ParametricEqTheme.h"

namespace parametric_eq::theme
{

/*---------------------------------------------------------------------------
**
*/
const juce::Colour DARK_GREY = { 37u, 33u, 38u };
const juce::Colour PARCHMENT = { 250u, 244u, 237u };
const juce::Colour RED       = { 214u, 132u, 129u };
const juce::Colour TEAL      = { 96u, 153u, 162u };
const juce::Colour PURPLE    = { 103u, 96u, 162u };
const juce::Colour GREEN     = { 96u, 162u, 107u };
const juce::Colour ACCENT    = TEAL;

/*---------------------------------------------------------------------------
**
*/
const juce::Colour MAIN_BACKGROUND         = DARK_GREY;
const juce::Colour TEXT                    = PARCHMENT.withAlpha(0.7f);
const juce::Colour TEXT_DISABLED           = TEXT.withAlpha(DISABLED_ALPHA);
const juce::Colour ANALYSER_TEXT           = TEXT;
const juce::Colour RESPONSE_CURVE_FILL     = ACCENT.withAlpha(0.5f);
const juce::Colour RESPONSE_CURVE_DISABLED = PARCHMENT.withAlpha(0.05f);
const juce::Colour RESPONSE_CURVE_AVG      = ACCENT;
const juce::Colour NODE_ON                 = ACCENT;
const juce::Colour NODE_OFF                = PARCHMENT.withAlpha(0.5f);
const juce::Colour ANALYSER_GRID           = PARCHMENT;
const juce::Colour ANALYSER_WAVEFORM       = PARCHMENT.withAlpha(0.05f);
const juce::Colour ACTIVE_BAND_BACKGROUND  = PARCHMENT.withAlpha(0.025f);
const juce::Colour TEXT_BUTTON_BG_HOVER    = PARCHMENT.withAlpha(0.025f);
const juce::Colour BUTTON_ICON             = ACCENT;
const juce::Colour BUTTON_ICON_DISABLED    = TEXT_DISABLED;

/*---------------------------------------------------------------------------
**
*/
juce::Font
getFont()
{
    return juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 11.f, juce::Font::plain);
}

}  // namespace parametric_eq::theme

/*---------------------------------------------------------------------------
** End of File
*/
