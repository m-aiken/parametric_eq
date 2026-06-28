#include "AudioCallbackListener.h"

namespace
{
constexpr std::chrono::milliseconds::rep TIMEOUT_THRESHOLD = 120;
}

/*---------------------------------------------------------------------------
**
*/
AudioCallbackListener::AudioCallbackListener()
    : last_audio_callback_(TIMEOUT_THRESHOLD)
{
}

/*---------------------------------------------------------------------------
**
*/
void
AudioCallbackListener::bump()
{
    last_audio_callback_.set(currentTime());
}

/*---------------------------------------------------------------------------
**
*/
bool
AudioCallbackListener::callbackIsActive() const
{
    return (currentTime() - last_audio_callback_.get()) < TIMEOUT_THRESHOLD;
}

/*---------------------------------------------------------------------------
**
*/
/*static*/ std::chrono::milliseconds::rep
AudioCallbackListener::currentTime()
{
    const auto current_time = std::chrono::system_clock::now();
    const auto since_epoch  = current_time.time_since_epoch();

    return std::chrono::duration_cast< std::chrono::milliseconds >(since_epoch).count();
}

/*---------------------------------------------------------------------------
** End of File
*/
