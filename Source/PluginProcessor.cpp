#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
constexpr float maxDelaySeconds = 0.030f;
constexpr float centerDelaySeconds = 0.010f;
constexpr float lowDepthExcursionSeconds = 0.0012f;
constexpr float highDepthExcursionSeconds = 0.0060f;
constexpr float bbdBandwidthHz = 3400.0f;
constexpr float inputHighPassHz = 35.0f;
constexpr float twoPi = juce::MathConstants<float>::twoPi;

juce::AudioParameterFloat* floatParameter (juce::AudioProcessorValueTreeState& state, const char* id)
{
    return dynamic_cast<juce::AudioParameterFloat*> (state.getParameter (id));
}
}

SmallCloneCircuitChorusAudioProcessor::SmallCloneCircuitChorusAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SmallCloneCircuitChorusAudioProcessor::createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> layout;
    layout.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "rate", 1 }, "Rate", NormalisableRange<float> (0.05f, 8.0f, 0.0f, 0.42f), 0.8f, "Hz"));
    layout.push_back (std::make_unique<AudioParameterChoice> (
        ParameterID { "depth", 1 }, "Depth", StringArray { "Low", "High" }, 1));
    layout.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { "mix", 1 }, "Mix", NormalisableRange<float> (0.0f, 1.0f), 0.5f));
    layout.push_back (std::make_unique<AudioParameterBool> (
        ParameterID { "bypass", 1 }, "Pedal Bypass", false));
    layout.push_back (std::make_unique<AudioParameterBool> (
        ParameterID { "sync", 1 }, "Rate Sync", false));
    return { layout.begin(), layout.end() };
}

bool SmallCloneCircuitChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();
    return input == output && (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo());
}

void SmallCloneCircuitChorusAudioProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    juce::ignoreUnused (maximumExpectedSamplesPerBlock);
    currentSampleRate = sampleRate;
    delayLength = juce::jmax (8, (int) std::ceil (sampleRate * maxDelaySeconds) + 4);

    const auto lowpass = juce::dsp::IIR::Coefficients<float>::makeLowPass (sampleRate, bbdBandwidthHz, 0.7071f);
    const auto highpass = juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, inputHighPassHz, 0.7071f);

    for (auto& channel : channels)
    {
        channel.delay.setSize (1, delayLength, false, true, true);
        channel.delay.clear();
        channel.writePosition = 0;
        channel.lfoPhase = 0.0f;
        channel.inputFilterA.coefficients = highpass;
        channel.inputFilterB.coefficients = lowpass;
        channel.outputFilterA.coefficients = lowpass;
        channel.outputFilterB.coefficients = lowpass;
        channel.inputFilterA.reset();
        channel.inputFilterB.reset();
        channel.outputFilterA.reset();
        channel.outputFilterB.reset();
    }
}

void SmallCloneCircuitChorusAudioProcessor::releaseResources()
{
    for (auto& channel : channels)
        resetChannel (channel);
}

void SmallCloneCircuitChorusAudioProcessor::resetChannel (ChannelState& channel)
{
    channel.delay.clear();
    channel.writePosition = 0;
    channel.lfoPhase = 0.0f;
    channel.inputFilterA.reset();
    channel.inputFilterB.reset();
    channel.outputFilterA.reset();
    channel.outputFilterB.reset();
}

float SmallCloneCircuitChorusAudioProcessor::readFractionalDelay (const ChannelState& channel, float delaySamples)
{
    const auto length = channel.delay.getNumSamples();
    auto position = (float) channel.writePosition - delaySamples;
    while (position < 0.0f)
        position += (float) length;
    while (position >= (float) length)
        position -= (float) length;

    const auto index0 = (int) position;
    const auto fraction = position - (float) index0;
    const auto index1 = (index0 + 1) % length;
    const auto* data = channel.delay.getReadPointer (0);

    // Cubic interpolation keeps the moving BBD delay smoother than a linear tap.
    const auto indexM1 = (index0 + length - 1) % length;
    const auto index2 = (index0 + 2) % length;
    const auto y0 = data[indexM1];
    const auto y1 = data[index0];
    const auto y2 = data[index1];
    const auto y3 = data[index2];
    const auto c0 = y1;
    const auto c1 = 0.5f * (y2 - y0);
    const auto c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    const auto c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
    return ((c3 * fraction + c2) * fraction + c1) * fraction + c0;
}

void SmallCloneCircuitChorusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused (midi);
    const auto inputChannels = getTotalNumInputChannels();
    const auto outputChannels = getTotalNumOutputChannels();
    for (auto channel = inputChannels; channel < outputChannels; ++channel)
        buffer.clear (channel, 0, buffer.getNumSamples());

    const auto* rateParameter = floatParameter (parameters, "rate");
    const auto* mixParameter = floatParameter (parameters, "mix");
    auto rateHz = rateParameter != nullptr ? rateParameter->get() : 0.8f;
    const auto mix = mixParameter != nullptr ? mixParameter->get() : 0.5f;
    const auto depthMode = (int) parameters.getRawParameterValue ("depth")->load();
    const auto bypass = parameters.getRawParameterValue ("bypass")->load() >= 0.5f;
    const auto sync = parameters.getRawParameterValue ("sync")->load() >= 0.5f;

    if (sync)
    {
        if (auto* playHead = getPlayHead())
        {
            if (const auto position = playHead->getPosition())
            {
                if (const auto bpm = position->getBpm())
                {
                    constexpr std::array<float, 15> beatDivisions {
                        16.0f, 12.0f, 8.0f, 6.0f, 4.0f, 3.0f, 2.0f, 1.5f,
                        1.0f, 0.75f, 0.5f, 0.375f, 0.25f, 0.1875f, 0.125f
                    };
                    const auto targetRate = rateHz;
                    auto closestDistance = std::numeric_limits<float>::max();
                    for (const auto beats : beatDivisions)
                    {
                        const auto candidate = juce::jlimit (0.05f, 8.0f, (float) *bpm / (60.0f * beats));
                        const auto distance = std::abs (std::log (candidate / targetRate));
                        if (distance < closestDistance)
                        {
                            closestDistance = distance;
                            rateHz = candidate;
                        }
                    }
                }
            }
        }
    }
    const auto excursion = depthMode == 0 ? lowDepthExcursionSeconds : highDepthExcursionSeconds;
    const auto increment = twoPi * rateHz / (float) currentSampleRate;
    const auto dryGain = std::cos (mix * juce::MathConstants<float>::halfPi);
    const auto wetGain = std::sin (mix * juce::MathConstants<float>::halfPi);

    for (int channelIndex = 0; channelIndex < inputChannels; ++channelIndex)
    {
        auto& state = channels[(size_t) juce::jmin (channelIndex, 1)];
        auto* samples = buffer.getWritePointer (channelIndex);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const auto dry = samples[i];
            const auto conditioned = state.inputFilterB.processSample (state.inputFilterA.processSample (dry));
            state.delay.setSample (0, state.writePosition, conditioned);

            const auto lfo = std::sin (state.lfoPhase);
            const auto delaySeconds = juce::jlimit (0.002f, maxDelaySeconds - 0.001f,
                                                     centerDelaySeconds + excursion * lfo);
            const auto wetRaw = readFractionalDelay (state, delaySeconds * (float) currentSampleRate);
            const auto wetFiltered = state.outputFilterB.processSample (state.outputFilterA.processSample (wetRaw));

            if (bypass)
                samples[i] = dry;
            else
                samples[i] = dryGain * dry + wetGain * wetFiltered;

            state.writePosition = (state.writePosition + 1) % delayLength;
            state.lfoPhase += increment;
            if (state.lfoPhase >= twoPi)
                state.lfoPhase -= twoPi;
        }
    }
}

juce::AudioProcessorEditor* SmallCloneCircuitChorusAudioProcessor::createEditor()
{
    return new SmallCloneCircuitChorusAudioProcessorEditor (*this);
}

void SmallCloneCircuitChorusAudioProcessor::getStateInformation (juce::MemoryBlock& destinationData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary (*xml, destinationData);
}

void SmallCloneCircuitChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SmallCloneCircuitChorusAudioProcessor();
}
