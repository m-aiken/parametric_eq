#pragma once

#include <JuceHeader.h>

namespace parametric_eq
{

inline constexpr float MIN_HZ = 20.f;
inline constexpr float MAX_HZ = 20000.f;
inline constexpr float MIN_DB = -12.f;
inline constexpr float MAX_DB = 12.f;

// Plugin bypass parameter.
inline const juce::String PLUGIN_BYPASS_PARAMETER = "Bypass";
inline constexpr bool     PLUGIN_BYPASS_DEFAULT   = false;

// Filter constants.
using Filter = juce::dsp::IIR::Filter< float >;

// Cut filter chain: x4 filters to allow for 12/24/36/48 dB/oct options.
using CutFilter = juce::dsp::ProcessorChain< Filter, Filter, Filter, Filter >;

using FilterChain = juce::dsp::ProcessorChain< CutFilter,  //! Low Cut.
                                               Filter,     //! Low Shelf.
                                               Filter,     //! Peak Band 1.
                                               Filter,     //! Peak Band 2.
                                               Filter,     //! Peak Band 3.
                                               Filter,     //! Peak Band 4.
                                               Filter,     //! Peak Band 5.
                                               Filter,     //! High Shelf.
                                               CutFilter   //! High Cut.
                                               >;

inline constexpr int NUM_FILTERS           = 9;
inline constexpr int NUM_PEAK_BANDS        = 5;
inline constexpr int NUM_CUT_SLOPE_OPTIONS = 4;

enum FILTER_ID {
    LOW_CUT,
    LOW_SHELF,
    PEAK_BAND_1,
    PEAK_BAND_2,
    PEAK_BAND_3,
    PEAK_BAND_4,
    PEAK_BAND_5,
    HIGH_SHELF,
    HIGH_CUT,
    NULL_BAND,
};

enum CUT_FILTER_SLOPE {
    SLOPE_12_DBPO,
    SLOPE_24_DBPO,
    SLOPE_36_DBPO,
    SLOPE_48_DBPO,
};

struct FilterParameterIds {
    juce::String enabled_;
    juce::String frequency_;
    juce::String gain_;
    juce::String q_;
    juce::String slope_;
};

// Filter parameters.
inline const juce::String PARAMETER_NOT_SUPPORTED = "PARAMETER_NOT_SUPPORTED";

inline const std::map< FILTER_ID, FilterParameterIds > PARAMETER_MAP = {
    { LOW_CUT,
      {
          .enabled_   = "Low Cut Enabled",
          .frequency_ = "Low Cut Frequency",
          .gain_      = PARAMETER_NOT_SUPPORTED,
          .q_         = PARAMETER_NOT_SUPPORTED,
          .slope_     = "Low Cut Slope",
      } },
    { LOW_SHELF,
      {
          .enabled_   = "Low Shelf Enabled",
          .frequency_ = "Low Shelf Frequency",
          .gain_      = "Low Shelf Gain",
          .q_         = PARAMETER_NOT_SUPPORTED,
          .slope_     = PARAMETER_NOT_SUPPORTED,
      } },
    { PEAK_BAND_1,
      {
          .enabled_   = "Peak Band 1 Enabled",
          .frequency_ = "Peak Band 1 Frequency",
          .gain_      = "Peak Band 1 Gain",
          .q_         = "Peak Band 1 Q",
          .slope_     = PARAMETER_NOT_SUPPORTED,
      } },
    { PEAK_BAND_2,
      {
          .enabled_   = "Peak Band 2 Enabled",
          .frequency_ = "Peak Band 2 Frequency",
          .gain_      = "Peak Band 2 Gain",
          .q_         = "Peak Band 2 Q",
          .slope_     = PARAMETER_NOT_SUPPORTED,
      } },
    { PEAK_BAND_3,
      {
          .enabled_   = "Peak Band 3 Enabled",
          .frequency_ = "Peak Band 3 Frequency",
          .gain_      = "Peak Band 3 Gain",
          .q_         = "Peak Band 3 Q",
          .slope_     = PARAMETER_NOT_SUPPORTED,
      } },
    { PEAK_BAND_4,
      {
          .enabled_   = "Peak Band 4 Enabled",
          .frequency_ = "Peak Band 4 Frequency",
          .gain_      = "Peak Band 4 Gain",
          .q_         = "Peak Band 4 Q",
          .slope_     = PARAMETER_NOT_SUPPORTED,
      } },
    { PEAK_BAND_5,
      {
          .enabled_   = "Peak Band 5 Enabled",
          .frequency_ = "Peak Band 5 Frequency",
          .gain_      = "Peak Band 5 Gain",
          .q_         = "Peak Band 5 Q",
          .slope_     = PARAMETER_NOT_SUPPORTED,
      } },
    { HIGH_SHELF,
      {
          .enabled_   = "High Shelf Enabled",
          .frequency_ = "High Shelf Frequency",
          .gain_      = "High Shelf Gain",
          .q_         = PARAMETER_NOT_SUPPORTED,
          .slope_     = PARAMETER_NOT_SUPPORTED,
      } },
    { HIGH_CUT,
      {
          .enabled_   = "High Cut Enabled",
          .frequency_ = "High Cut Frequency",
          .gain_      = PARAMETER_NOT_SUPPORTED,
          .q_         = PARAMETER_NOT_SUPPORTED,
          .slope_     = "High Cut Slope",
      } },
    { NULL_BAND,
      {
          .enabled_   = PARAMETER_NOT_SUPPORTED,
          .frequency_ = PARAMETER_NOT_SUPPORTED,
          .gain_      = PARAMETER_NOT_SUPPORTED,
          .q_         = PARAMETER_NOT_SUPPORTED,
          .slope_     = PARAMETER_NOT_SUPPORTED,
      } },
};

inline const juce::StringArray CUT_FILTER_SLOPE_CHOICES = {
    "12 dB/Oct",
    "24 dB/Oct",
    "36 dB/Oct",
    "48 dB/Oct",
};

inline constexpr bool  LOW_CUT_ENABLED_DEFAULT = false;
inline constexpr float LOW_CUT_FREQ_MIN        = MIN_HZ;
inline constexpr float LOW_CUT_FREQ_MAX        = 1000.f;
inline constexpr float LOW_CUT_FREQ_DEFAULT    = 50.f;

inline constexpr bool  LOW_SHELF_ENABLED_DEFAULT = false;
inline constexpr float LOW_SHELF_FREQ_MIN        = MIN_HZ;
inline constexpr float LOW_SHELF_FREQ_MAX        = 1000.f;
inline constexpr float LOW_SHELF_FREQ_DEFAULT    = 100.f;

inline constexpr bool  HIGH_CUT_ENABLED_DEFAULT = false;
inline constexpr float HIGH_CUT_FREQ_MIN        = 1000.f;
inline constexpr float HIGH_CUT_FREQ_MAX        = MAX_HZ;
inline constexpr float HIGH_CUT_FREQ_DEFAULT    = 15000.f;

inline constexpr bool  HIGH_SHELF_ENABLED_DEFAULT = false;
inline constexpr float HIGH_SHELF_FREQ_MIN        = 1000.f;
inline constexpr float HIGH_SHELF_FREQ_MAX        = MAX_HZ;
inline constexpr float HIGH_SHELF_FREQ_DEFAULT    = 8000.f;

inline constexpr int CUT_FILTER_SLOPE_DEFAULT = SLOPE_48_DBPO;  //! Same for low and high cut.

inline const juce::NormalisableRange LOW_CUT_FREQ_NORM_RANGE   = { LOW_CUT_FREQ_MIN, LOW_CUT_FREQ_MAX, 1.f, 0.4f };
inline const juce::NormalisableRange LOW_SHELF_FREQ_NORM_RANGE = { LOW_SHELF_FREQ_MIN, LOW_SHELF_FREQ_MAX, 1.f, 0.4f };
inline const juce::NormalisableRange HIGH_CUT_FREQ_NORM_RANGE  = { HIGH_CUT_FREQ_MIN, HIGH_CUT_FREQ_MAX, 1.f, 0.4f };
inline const juce::NormalisableRange HIGH_SHELF_FREQ_NORM_RANGE = { HIGH_SHELF_FREQ_MIN, HIGH_SHELF_FREQ_MAX, 1.f, 0.4f };

inline constexpr float SHELF_DB_MIN     = MIN_DB / 2.f;
inline constexpr float SHELF_DB_MAX     = MAX_DB / 2.f;
inline constexpr float SHELF_DB_DEFAULT = 0.f;
inline constexpr float SHELF_Q_DEFAULT  = 0.7f;

inline const juce::NormalisableRange SHELF_GAIN_NORMALISABLE_RANGE = { SHELF_DB_MIN, SHELF_DB_MAX, 0.01f, 1.f };

inline constexpr bool  PEAK_ENABLED_DEFAULT = true;
inline constexpr float PEAK_DB_MIN          = MIN_DB;
inline constexpr float PEAK_DB_MAX          = MAX_DB;
inline constexpr float PEAK_DB_DEFAULT      = 0.f;
inline constexpr float PEAK_Q_MIN           = 0.1f;
inline constexpr float PEAK_Q_MAX           = 10.f;
inline constexpr float PEAK_Q_DEFAULT       = 1.f;

inline const juce::NormalisableRange PEAK_FREQ_NORMALISABLE_RANGE = { MIN_HZ, MAX_HZ, 0.01f, 0.15f };
inline const juce::NormalisableRange PEAK_GAIN_NORMALISABLE_RANGE = { PEAK_DB_MIN, PEAK_DB_MAX, 0.01f, 1.f };
inline const juce::NormalisableRange PEAK_Q_NORMALISABLE_RANGE    = { PEAK_Q_MIN, PEAK_Q_MAX, 0.05f, 1.f };

inline constexpr std::array PEAK_BAND_DEFAULT_FREQUENCIES = {
    140.f,   //! Peak Band 1.
    320.f,   //! Peak Band 2.
    720.f,   //! Peak Band 3.
    1400.f,  //! Peak Band 4.
    3200.f,  //! Peak Band 5.
};

using APVTS = juce::AudioProcessorValueTreeState;
extern void addListenerToAllParameters(APVTS& apvts, APVTS::Listener* listener);
extern void removeListenerFromAllParameters(APVTS& apvts, APVTS::Listener* listener);

}  // namespace parametric_eq
