cmake_minimum_required(VERSION 3.22)
project(SmallCloneCircuitChorus VERSION 0.1.0)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.5
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(SmallCloneCircuitChorus
    COMPANY_NAME "Independent Audio"
    BUNDLE_ID "audio.independent.smallclonechorus"
    PLUGIN_MANUFACTURER_CODE IndA
    PLUGIN_CODE ScCh
    PRODUCT_NAME "Small Clone Circuit Chorus"
    DESCRIPTION "Circuit-informed Small Clone style chorus"
    FORMATS VST3 Standalone
    VST3_CATEGORIES Fx Modulation
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    HAS_EDITOR TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(SmallCloneCircuitChorus PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)
juce_generate_juce_header(SmallCloneCircuitChorus)

target_compile_features(SmallCloneCircuitChorus PRIVATE cxx_std_17)
target_compile_definitions(SmallCloneCircuitChorus PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(SmallCloneCircuitChorus PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags
)
