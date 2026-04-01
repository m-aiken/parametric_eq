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

struct ParamInfo {
    juce::String id_;
    juce::String gui_text_;
};

inline const ParamInfo LOW_CUT_FREQ     = { "Low Cut Freq", "Low Cut Freq" };
inline const ParamInfo LOW_CUT_SLOPE    = { "Low Cut Slope", "Low Cut Slope" };
inline const ParamInfo LOW_CUT_ENABLED  = { "Low Cut Enabled", "Low Cut Enabled" };
inline const ParamInfo HIGH_CUT_FREQ    = { "High Cut Freq", "High Cut Freq" };
inline const ParamInfo HIGH_CUT_SLOPE   = { "High Cut Slope", "High Cut Slope" };
inline const ParamInfo HIGH_CUT_ENABLED = { "High Cut Enabled", "High Cut Enabled" };

namespace FFT
{

    enum {
        SIZE_2048 = 11,
        SIZE_4096 = 12,
        SIZE_8192 = 13,
    };

    static constexpr int ORDER    = SIZE_8192;
    static constexpr int SIZE     = 1 << ORDER;
    static constexpr int NUM_BINS = SIZE / 2;

    static constexpr int INPUT_BUFFER_SIZE  = SIZE;
    static constexpr int OUTPUT_BUFFER_SIZE = SIZE * 2;

    typedef std::array< float, OUTPUT_BUFFER_SIZE > OutputBuffer;

    inline double getBinWidth(const double sample_rate)
    {
        return sample_rate / static_cast< double >(SIZE);
    }

}  // namespace FFT

}  // namespace GraphicEq
