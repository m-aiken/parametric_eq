#pragma once

#include <JuceHeader.h>

#include "Theme.h"

class GraphBackground final : public juce::Component
{
public:
    GraphBackground();

    void paint(juce::Graphics& g) override;

private:
    static bool  shouldDrawFrequency(const int hz);
    static float getAlphaForFrequency(const int hz);
    static void  drawDbLabels(const juce::Graphics& g, const juce::Rectangle< int >& bounds);
    static void  drawFrequencyLabels(const juce::Graphics& g, const juce::Rectangle< int >& bounds);

    static constexpr int DB_INTERVAL = 2;
    static constexpr int TEXT_WIDTH  = Theme::UI_PADDING * 16;

    static const std::vector< float > HZ_LIST;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphBackground)
};
