#pragma once

#include <array>

namespace parametric_eq::fft
{

enum {
    SIZE_2048 = 11,
    SIZE_4096 = 12,
    SIZE_8192 = 13,
};

inline constexpr float MIN_DB = -96.f;
inline constexpr float MAX_DB = 12.f;

inline constexpr int ORDER    = SIZE_8192;
inline constexpr int SIZE     = 1 << ORDER;
inline constexpr int NUM_BINS = SIZE / 2;

inline constexpr int INPUT_BUFFER_SIZE  = SIZE;
inline constexpr int OUTPUT_BUFFER_SIZE = SIZE * 2;

typedef std::array< float, OUTPUT_BUFFER_SIZE > OutputBuffer;

inline double
getBinWidth(const double sample_rate)
{
    return sample_rate / static_cast< double >(SIZE);
}

inline constexpr int   WAVEFORM_REFRESH_RATE_HZ          = 60;
inline constexpr int   WAVEFORM_PATH_RESOLUTION          = 2;
inline constexpr float WAVEFORM_DECAY_RATE_DB_PER_SECOND = 30.f;
inline constexpr float WAVEFORM_DECAY_RATE               = WAVEFORM_DECAY_RATE_DB_PER_SECOND / 60.f;

}  // namespace parametric_eq::fft
