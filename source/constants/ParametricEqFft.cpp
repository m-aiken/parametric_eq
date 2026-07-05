#include "ParametricEqFft.h"

namespace parametric_eq::fft
{

/*---------------------------------------------------------------------------
**
*/
double
getBinWidth(const double sample_rate)
{
    return sample_rate / static_cast< double >(SIZE);
}

}  // namespace parametric_eq::fft

/*---------------------------------------------------------------------------
** End of File
*/
