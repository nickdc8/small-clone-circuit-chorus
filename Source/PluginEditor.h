#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PedalLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosition, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;
    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool isHighlighted,
                           bool isDown) override;
};

class SmallCloneCircuitChorusAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit SmallCloneCircuitChorusAudioProcessorEditor (SmallCloneCircuitChorusAudioProcessor&);
    ~SmallCloneCircuitChorusAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    SmallCloneCircuitChorusAudioProcessor& pluginProcessor;
    PedalLookAndFeel pedalLook;
    juce::Slider rate;
    juce::ComboBox depth;
    juce::ToggleButton bypass;
    juce::ToggleButton sync { "SYNC" };
    std::unique_ptr<SliderAttachment> rateAttachment;
    std::unique_ptr<ComboAttachment> depthAttachment;
    std::unique_ptr<ButtonAttachment> bypassAttachment, syncAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmallCloneCircuitChorusAudioProcessorEditor)
};
