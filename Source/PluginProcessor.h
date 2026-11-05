#pragma once

#include <JuceHeader.h>

class SmallCloneCircuitChorusAudioProcessor final : public juce::AudioProcessor
{
public:
    SmallCloneCircuitChorusAudioProcessor();
    ~SmallCloneCircuitChorusAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.05; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destinationData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    struct ChannelState
    {
        juce::AudioBuffer<float> delay;
        int writePosition = 0;
        float lfoPhase = 0.0f;
        juce::dsp::IIR::Filter<float> inputFilterA, inputFilterB;
        juce::dsp::IIR::Filter<float> outputFilterA, outputFilterB;
    };

    void resetChannel (ChannelState& channel);
    static float readFractionalDelay (const ChannelState& channel, float delaySamples);

    std::array<ChannelState, 2> channels;
    double currentSampleRate = 44100.0;
    int delayLength = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SmallCloneCircuitChorusAudioProcessor)
};
