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

inline const juce::Font FONT = juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 10.f, juce::Font::plain);

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

inline const ParamInfo ANALYSER_POINTS = { "FFT Order", "Analyzer Points" };

}  // namespace GraphicEq
