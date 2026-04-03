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

inline constexpr int   WAVEFORM_REFRESH_RATE_HZ          = 60;
inline constexpr int   WAVEFORM_PATH_RESOLUTION          = 2;
inline constexpr float WAVEFORM_DECAY_RATE_DB_PER_SECOND = 30.f;
inline constexpr float WAVEFORM_DECAY_RATE               = WAVEFORM_DECAY_RATE_DB_PER_SECOND / 60.f;

struct ParamInfo {
    juce::String id_;
    juce::String gui_text_;
};

inline const ParamInfo LOW_CUT_ENABLED  = { "Low Cut Enabled", "Low Cut Enabled" };
inline const ParamInfo LOW_CUT_FREQ     = { "Low Cut Freq", "Low Cut Freq" };
inline const ParamInfo LOW_CUT_SLOPE    = { "Low Cut Slope", "Low Cut Slope" };
inline const ParamInfo HIGH_CUT_ENABLED = { "High Cut Enabled", "High Cut Enabled" };
inline const ParamInfo HIGH_CUT_FREQ    = { "High Cut Freq", "High Cut Freq" };
inline const ParamInfo HIGH_CUT_SLOPE   = { "High Cut Slope", "High Cut Slope" };

enum CUT_FILTER_SLOPE {
    SLOPE_12_DB_PER_OCTAVE,
    SLOPE_24_DB_PER_OCTAVE,
    SLOPE_36_DB_PER_OCTAVE,
    SLOPE_48_DB_PER_OCTAVE,
};

inline const juce::StringArray CUT_FILTER_SLOPE_CHOICES = {
    "12 dB/Oct",
    "24 dB/Oct",
    "36 dB/Oct",
    "48 dB/Oct",
};

inline constexpr bool  LOW_CUT_ENABLED_DEFAULT = false;
inline constexpr float LOW_CUT_FREQ_MIN        = MIN_HZ;
inline constexpr float LOW_CUT_FREQ_MAX        = 2000.f;
inline constexpr float LOW_CUT_FREQ_DEFAULT    = 50.f;
inline constexpr int   LOW_CUT_SLOPE_DEFAULT   = SLOPE_48_DB_PER_OCTAVE;

inline constexpr bool  HIGH_CUT_ENABLED_DEFAULT = false;
inline constexpr float HIGH_CUT_FREQ_MIN        = 500.f;
inline constexpr float HIGH_CUT_FREQ_MAX        = MAX_HZ;
inline constexpr float HIGH_CUT_FREQ_DEFAULT    = 15000.f;
inline constexpr int   HIGH_CUT_SLOPE_DEFAULT   = SLOPE_48_DB_PER_OCTAVE;

inline const juce::NormalisableRange LOW_CUT_FREQ_NORMALISABLE_RANGE = { LOW_CUT_FREQ_MIN, LOW_CUT_FREQ_MAX, 1.f, 0.5f };
inline const juce::NormalisableRange HIGH_CUT_FREQ_NORMALISABLE_RANGE = { HIGH_CUT_FREQ_MIN, HIGH_CUT_FREQ_MAX, 1.f, 0.2f };

enum PEAK_CONTROL {
    PEAK_ENABLED,
    PEAK_FREQ,
    PEAK_GAIN,
    PEAK_Q,
};

inline const juce::StringArray PEAK_CONTROL_LABELS = {
    "Enabled",
    "Freq",
    "Gain",
    "Q",
};

inline juce::String
getPeakParamName(const PEAK_CONTROL& peak_control, const int band_index)
{
    return { "P" + juce::String(band_index) + " " + PEAK_CONTROL_LABELS[peak_control] };
}
#if 0
inline constexpr std::vector< float > TARGET_FREQUENCIES = {
    MIN_HZ, 50.f, 100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f, 10000.f, MAX_HZ,
};
#endif

inline constexpr std::array< float, NUM_PEAK_BANDS > PEAK_BAND_CENTRE_FREQUENCIES = {
    100.f, 200.f, 500.f, 1000.f, 2000.f, 5000.f,
};

inline constexpr bool  PEAK_ENABLED_DEFAULT = true;
inline constexpr float PEAK_FREQ_MIN        = MIN_HZ;
inline constexpr float PEAK_FREQ_MAX        = MAX_HZ;
inline constexpr float PEAK_GAIN_MIN        = MIN_DB;
inline constexpr float PEAK_GAIN_MAX        = MAX_DB;
inline constexpr float PEAK_GAIN_DEFAULT    = 0.f;
inline constexpr float PEAK_Q_MIN           = 0.1f;
inline constexpr float PEAK_Q_MAX           = 10.f;
inline constexpr float PEAK_Q_DEFAULT       = 1.f;

inline const juce::NormalisableRange PEAK_FREQ_NORMALISABLE_RANGE = { PEAK_FREQ_MIN, PEAK_FREQ_MAX, 1.f, 0.1f };
inline const juce::NormalisableRange PEAK_GAIN_NORMALISABLE_RANGE = { PEAK_GAIN_MIN, PEAK_GAIN_MAX, 0.5f, 1.f };
inline const juce::NormalisableRange PEAK_Q_NORMALISABLE_RANGE    = { PEAK_Q_MIN, PEAK_Q_MAX, 0.05f, 1.f };

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
