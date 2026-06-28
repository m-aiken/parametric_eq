#pragma once

#include <JuceHeader.h>

// Hosts vary in the way that they handle plugin bypass.
// Some will set the parameter returned by the plugin's getBypassParameter() method (preferred).
// Others will simply stop calling the plugin's processBlock() method.
// Prefer the former but fall back to the latter by way of this timeout heuristic:
// * The plugin should have one AudioCallbackListener member in its processor class.
// * The plugin calls bump() on each audio callback (from its processBlock()).
//   - bump() records the current time.
// * Interested classes (usually GUI related) can query callbackIsActive().
//   - callbackIsActive() returns whether the host was last seen within an acceptable threshold.

class AudioCallbackListener final
{
public:
    AudioCallbackListener();

    void bump();
    bool callbackIsActive() const;

private:
    static std::chrono::milliseconds::rep currentTime();

    juce::Atomic< std::chrono::milliseconds::rep > last_audio_callback_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCallbackListener)
};
