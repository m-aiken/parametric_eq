#pragma once

#include <JuceHeader.h>

namespace GraphicEq
{

inline constexpr float MIN_HZ         = 20.f;
inline constexpr float MAX_HZ         = 20000.f;
inline constexpr float MIN_DB         = -72.f;
inline constexpr float MAX_DB         = 24.f;
inline constexpr int   MAX_NUM_BANDS  = 8;
inline constexpr int   MIN_BAND_INDEX = 0;
inline constexpr int   MAX_BAND_INDEX = 7;
inline constexpr int   NUM_PEAK_BANDS = 6;

inline constexpr float BORDER_CORNER_RADIUS = 5.f;
inline constexpr float BORDER_THICKNESS     = 2.f;

static constexpr float WIDGET_ALPHA_FULL = 1.f;
static constexpr float WIDGET_ALPHA_DIM  = 0.2f;

enum Channel {
    Left,
    Right,
};

enum FFTOrder {
    order2048 = 11,
    order4096 = 12,
    order8192 = 13,
};

inline const std::map< GraphicEq::FFTOrder, juce::String > FTT_ORDERS = { { order2048, "2048" },
                                                                          { order4096, "4096" },
                                                                          { order8192, "8192" } };

struct ParamInfo {
    juce::String id_;
    juce::String gui_text_;
};

inline const ParamInfo ANALYSER_POINTS  = { "FFT Order", "Analyzer Points" };
inline const ParamInfo LOW_CUT_FREQ     = { "Low Cut Freq", "Low Cut Freq" };
inline const ParamInfo LOW_CUT_SLOPE    = { "Low Cut Slope", "Low Cut Slope" };
inline const ParamInfo LOW_CUT_ENABLED  = { "Low Cut Enabled", "Low Cut Enabled" };
inline const ParamInfo HIGH_CUT_FREQ    = { "High Cut Freq", "High Cut Freq" };
inline const ParamInfo HIGH_CUT_SLOPE   = { "High Cut Slope", "High Cut Slope" };
inline const ParamInfo HIGH_CUT_ENABLED = { "High Cut Enabled", "High Cut Enabled" };

}  // namespace GraphicEq
