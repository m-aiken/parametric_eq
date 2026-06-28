#pragma once

#include <JuceHeader.h>

#include "ParametricEqProcessor.h"
#include "ParametricEqTheme.h"

class GraphBackground final : public juce::Component
{
public:
    explicit GraphBackground(ParametricEqProcessor& processor_ref);

    void paint(juce::Graphics& g) override;

private:
    void drawDbLabels(const juce::Graphics& g) const;
    void drawFrequencyLabels(const juce::Graphics& g) const;

    static float getAlphaForDb(const int db);
    static float getAlphaForFrequency(const int hz);
    static bool  shouldDrawFrequency(const int hz);

    static constexpr int              TEXT_WIDTH         = parametric_eq::theme::UI_PADDING * 16;
    static constexpr float            LINE_ALPHA_BOLDER  = 0.1f;
    static constexpr float            LINE_ALPHA_LIGHTER = 0.05f;
    static constexpr int              DB_LABEL_INTERVAL  = 3;
    static const std::vector< float > HZ_LIST;

    ParametricEqProcessor& processor_ref_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphBackground)
};
