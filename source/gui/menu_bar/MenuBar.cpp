#include "MenuBar.h"

/*---------------------------------------------------------------------------
**
*/
MenuBar::MenuBar(ParametricEqProcessor& processor_ref)
    : disable_button_(processor_ref)
    , waveform_button_(processor_ref)
{
    addAndMakeVisible(disable_button_);
    addAndMakeVisible(waveform_button_);
}

/*---------------------------------------------------------------------------
**
*/
void
MenuBar::resized()
{
    const auto& bounds        = getLocalBounds();
    const auto& bounds_height = bounds.getHeight();
    const auto  button_width  = static_cast< int >(bounds_height * 1.75f);

    disable_button_.setBounds(0, 0, button_width, bounds_height);
    waveform_button_.setBounds(bounds.getRight() - button_width, 0, button_width, bounds_height);
}

/*---------------------------------------------------------------------------
** End of File
*/
