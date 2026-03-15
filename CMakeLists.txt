cmake_minimum_required(VERSION 3.23)
project(GRAPHIC_EQ VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(JUCE_COPY_PLUGIN_AFTER_BUILD ON)

#
# Dependencies.
# 1. Create a dependencies folder in our project.
# 2. Get CPM (cmake package manager).
# 3. Use CPM to get JUCE.
#
set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
include(cmake/get_cpm.cmake)
include(cmake/get_juce.cmake)

#
# Plugin configuration.
#
juce_add_plugin(${PROJECT_NAME}
        # VERSION ...                               # Set this if the plugin version is different to the project version
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME maiken                         # Specify the name of the plugin's author
        IS_SYNTH FALSE                              # Is this a synth or an effect?
        NEEDS_MIDI_INPUT FALSE                      # Does the plugin need midi input?
        NEEDS_MIDI_OUTPUT FALSE                     # Does the plugin need midi output?
        IS_MIDI_EFFECT FALSE                        # Is this plugin a MIDI effect?
        # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
        # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE        # Should the plugin be installed to a default location after building?
        PLUGIN_MANUFACTURER_CODE MAPC               # A four-character manufacturer id with at least one upper-case character
        PLUGIN_CODE P005                            # A unique four-character plugin id with exactly one upper-case character
        # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
        FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
        PRODUCT_NAME "Graphic EQ")                  # The name of the final executable, which can differ from the target name

juce_generate_juce_header(${PROJECT_NAME})

set(PROJECT_SOURCES
        PRIVATE
        source/PluginEditor.cpp
        source/PluginProcessor.cpp
        source/dsp/analyzerpathgenerator.cpp
        source/dsp/fftdatagenerator.cpp
        source/dsp/filterutils.cpp
        source/gui/analyzer/analyzercontainer.cpp
        source/gui/analyzer/responsecurve.cpp
        source/gui/analyzer/spectrumanalyzer.cpp
        source/gui/analyzer/spectrumgrid.cpp
        source/gui/analyzercontrols.cpp
        source/gui/bandcontrols/cutcontrol.cpp
        source/gui/bandcontrols/eqcontrolscontainer.cpp
        source/gui/bandcontrols/peakcontrol.cpp
        source/gui/customlookandfeel.cpp
        source/gui/pathproducer.cpp
        source/gui/widgets/customlinearslider.cpp
        source/gui/widgets/customrotarycontrol.cpp
        source/gui/widgets/powerbutton.cpp
        source/gui/widgets/themebutton.cpp
)

set(PROJECT_HEADERS
        PRIVATE
        source/PluginEditor.h
        source/PluginProcessor.h
        source/utils/colourpalette.h
        source/utils/globals.h
        source/utils/tailwindcolours.h
        source/dsp/analyzerpathgenerator.h
        source/dsp/analyzerproperties.h
        source/dsp/eqproperties.h
        source/dsp/fftdatagenerator.h
        source/dsp/fifo.h
        source/dsp/filterutils.h
        source/dsp/monobufferfifo.h
        source/dsp/peakband.h
        source/gui/analyzer/analyzercontainer.h
        source/gui/analyzer/responsecurve.h
        source/gui/analyzer/spectrumanalyzer.h
        source/gui/analyzer/spectrumgrid.h
        source/gui/analyzercontrols.h
        source/gui/bandcontrols/cutcontrol.h
        source/gui/bandcontrols/eqcontrolscontainer.h
        source/gui/bandcontrols/peakcontrol.h
        source/gui/customlookandfeel.h
        source/gui/paramlistener.h
        source/gui/pathproducer.h
        source/gui/widgets/customlinearslider.h
        source/gui/widgets/customrotarycontrol.h
        source/gui/widgets/powerbutton.h
        source/gui/widgets/themebutton.h
)

target_sources(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/source/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/source/dsp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/gui
        ${CMAKE_CURRENT_SOURCE_DIR}/source/gui/analyzer
        ${CMAKE_CURRENT_SOURCE_DIR}/source/gui/bandcontrols
        ${CMAKE_CURRENT_SOURCE_DIR}/source/gui/widgets
)

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_add_binary_data(AudioPluginData SOURCES resources/day_and_night_icon.png)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        AudioPluginData
        juce::juce_audio_utils
        juce::juce_dsp
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
