#pragma once

#include <JuceHeader.h>

template < typename T >
struct ParamListener : juce::Timer {
    ParamListener(juce::RangedAudioParameter& rap, std::function< void(T) > valueChangedHandler)
        : param(&rap)
        , handleValueChanged(valueChangedHandler)
    {
        jassert(param != nullptr);
        jassert(handleValueChanged != nullptr);

        paramValue = param->getValue();
        startTimerHz(60);
    }

    ~ParamListener() { stopTimer(); }

    void timerCallback() override
    {
        if (param == nullptr) {
            return;
        }

        auto newValue = param->getValue();
        if (newValue != paramValue) {
            paramValue = newValue;

            handleValueChanged(param->convertFrom0to1(paramValue));
        }
    }

private:
    T                           paramValue;
    juce::RangedAudioParameter* param;
    std::function< void(T) >    handleValueChanged;
};
