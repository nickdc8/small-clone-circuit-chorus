#include "PluginEditor.h"

namespace
{
const juce::Colour ink { 15, 16, 17 };
const juce::Colour cream { 232, 229, 217 };
const juce::Colour metal { 190, 188, 179 };

void drawScrew (juce::Graphics& g, float x, float y)
{
    g.setColour (juce::Colour (0x55000000));
    g.fillEllipse (x - 8.0f, y - 7.0f, 16.0f, 16.0f);
    g.setColour (juce::Colour (0xff8b8980));
    g.fillEllipse (x - 6.5f, y - 6.5f, 13.0f, 13.0f);
    g.setColour (juce::Colour (0xffc5c2b9));
    g.drawEllipse (x - 5.5f, y - 5.5f, 11.0f, 11.0f, 1.0f);
    g.setColour (juce::Colour (0xff5a5955));
    g.drawLine (x - 3.0f, y + 2.5f, x + 3.0f, y - 2.5f, 1.4f);
}
}

void PedalLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPosition, float rotaryStartAngle,
                                          float rotaryEndAngle, juce::Slider&)
{
    const auto side = (float) juce::jmin (width, height);
    const auto bounds = juce::Rectangle<float> ((float) x + ((float) width - side) * 0.5f,
                                                 (float) y + ((float) height - side) * 0.5f,
                                                 side, side).reduced (5.0f);
    const auto centre = bounds.getCentre();
    const auto radius = bounds.getWidth() * 0.5f;
    const auto angle = rotaryStartAngle + sliderPosition * (rotaryEndAngle - rotaryStartAngle);

    g.setColour (juce::Colour (0x55000000));
    g.fillEllipse (bounds.translated (0.0f, 4.0f));
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff555653), bounds.getX(), bounds.getY(),
                                             juce::Colour (0xff171818), bounds.getRight(), bounds.getBottom(), false));
    g.fillEllipse (bounds);
    g.setColour (juce::Colour (0xff696b67));
    g.drawEllipse (bounds, 1.2f);
    g.setColour (juce::Colour (0xff111212));
    g.fillEllipse (bounds.reduced (radius * 0.19f));
    g.setColour (juce::Colour (0xff3e403e));
    g.drawEllipse (bounds.reduced (radius * 0.19f), 1.0f);

    const auto pointerLength = radius * 0.61f;
    const auto pointerWidth = juce::jmax (2.0f, radius * 0.07f);
    juce::Path pointer;
    pointer.addRoundedRectangle (-pointerWidth * 0.5f, -radius * 0.77f, pointerWidth,
                                 pointerLength, pointerWidth * 0.5f);
    g.setColour (cream);
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
}

void PedalLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                      int, int, int, int, juce::ComboBox& box)
{
    const auto bounds = juce::Rectangle<float> (0.0f, 0.0f, (float) width, (float) height).reduced (3.0f);
    g.setColour (juce::Colour (0xff101112));
    g.fillRoundedRectangle (bounds, 5.0f);
    g.setColour (juce::Colour (0xff686a67));
    g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

    const auto selectedHigh = box.getSelectedId() == 2;
    const auto slot = bounds.reduced (7.0f, 8.0f);
    g.setColour (juce::Colour (0xff080909));
    g.fillRoundedRectangle (slot, 3.0f);
    const auto thumbHeight = (slot.getHeight() - 3.0f) * 0.5f;
    auto thumb = slot.withY (selectedHigh ? slot.getY() + 1.0f : slot.getY() + thumbHeight + 2.0f)
                     .withHeight (thumbHeight);
    g.setGradientFill (juce::ColourGradient (isButtonDown ? metal.brighter (0.1f) : metal,
                                             thumb.getX(), thumb.getY(),
                                             juce::Colour (0xff686a67), thumb.getRight(), thumb.getBottom(), false));
    g.fillRoundedRectangle (thumb, 2.0f);
    g.setColour (juce::Colour (0xff4b4c49));
    g.drawLine (thumb.getCentreX() - 7.0f, thumb.getCentreY(), thumb.getCentreX() + 7.0f,
                thumb.getCentreY(), 1.0f);
}

void PedalLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                         bool isHighlighted, bool isDown)
{
    if (button.getButtonText() == "SYNC")
    {
        auto bounds = button.getLocalBounds().toFloat();
        g.setColour (cream);
        g.setFont (juce::FontOptions (14.0f, juce::Font::bold));
        g.drawText ("SYNC", bounds.removeFromLeft (62.0f), juce::Justification::centredLeft);

        auto pill = button.getLocalBounds().toFloat().withTrimmedLeft (66.0f).reduced (2.0f, 8.0f);
        g.setColour (juce::Colour (0xffc4c4c0));
        g.fillRoundedRectangle (pill, pill.getHeight() * 0.5f);
        g.setColour (juce::Colour (0xff151616));
        g.fillRoundedRectangle (pill.reduced (2.0f), pill.getHeight() * 0.5f);
        const auto on = button.getToggleState();
        const auto labelArea = pill.withTrimmedLeft (6.0f).withTrimmedRight (pill.getWidth() * 0.45f);
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.setColour (on ? cream : cream.withAlpha (0.48f));
        g.drawText (on ? "ON" : "OFF", labelArea, juce::Justification::centred);
        g.setColour (on ? juce::Colour (0xff74c89e) : juce::Colour (0xff71736f));
        const auto thumbX = on ? pill.getRight() - pill.getHeight() + 3.0f : pill.getX() + 3.0f;
        g.fillEllipse (thumbX, pill.getY() + 3.0f, pill.getHeight() - 6.0f, pill.getHeight() - 6.0f);
        if (isHighlighted || button.hasKeyboardFocus (true))
        {
            g.setColour (juce::Colour (0xff9ad6ba).withAlpha (0.65f));
            g.drawRoundedRectangle (pill.expanded (2.0f), pill.getHeight() * 0.5f, 1.0f);
        }
        return;
    }

    auto bounds = button.getLocalBounds().toFloat().reduced (6.0f);
    const auto radius = bounds.getWidth() * 0.5f;
    auto face = bounds.withSizeKeepingCentre (radius * 1.36f, radius * 1.36f);
    g.setColour (juce::Colour (0x99000000));
    g.fillEllipse (face.translated (0.0f, isDown ? 2.0f : 4.0f));
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff0eee6), face.getX(), face.getY(),
                                             juce::Colour (0xff74736d), face.getRight(), face.getBottom(), false));
    g.fillEllipse (face);
    g.setColour (juce::Colour (0xff474743));
    g.drawEllipse (face.reduced (2.0f), 1.2f);
    auto cap = face.reduced (face.getWidth() * 0.21f);
    if (button.getToggleState())
        cap = cap.translated (0.0f, 2.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffb7b5ae), cap.getX(), cap.getY(),
                                             juce::Colour (0xff777771), cap.getRight(), cap.getBottom(), false));
    g.fillEllipse (cap);
    g.setColour (juce::Colour (0xff555650));
    g.drawEllipse (cap, 1.0f);
    if (isHighlighted || button.hasKeyboardFocus (true))
    {
        g.setColour (juce::Colour (0xffb9d7c6));
        g.drawEllipse (bounds.expanded (1.0f), 1.0f);
    }
}

SmallCloneCircuitChorusAudioProcessorEditor::SmallCloneCircuitChorusAudioProcessorEditor (
    SmallCloneCircuitChorusAudioProcessor& p)
    : AudioProcessorEditor (&p), pluginProcessor (p)
{
    setSize (900, 640);
    setLookAndFeel (&pedalLook);
    rate.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    rate.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    rate.setRotaryParameters (juce::degreesToRadians (228.0f), juce::degreesToRadians (492.0f), true);
    depth.addItem ("Low", 1);
    depth.addItem ("High", 2);
    bypass.setButtonText ({ });
    bypass.setClickingTogglesState (true);

    const std::array<juce::Component*, 4> controls { &rate, &depth, &bypass, &sync };
    for (auto* component : controls)
        addAndMakeVisible (component);

    rateAttachment = std::make_unique<SliderAttachment> (pluginProcessor.parameters, "rate", rate);
    depthAttachment = std::make_unique<ComboAttachment> (pluginProcessor.parameters, "depth", depth);
    bypassAttachment = std::make_unique<ButtonAttachment> (pluginProcessor.parameters, "bypass", bypass);
    syncAttachment = std::make_unique<ButtonAttachment> (pluginProcessor.parameters, "sync", sync);
    bypass.onStateChange = [this] { repaint (540, 90, 95, 60); };
}

SmallCloneCircuitChorusAudioProcessorEditor::~SmallCloneCircuitChorusAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void SmallCloneCircuitChorusAudioProcessorEditor::paint (juce::Graphics& graphics)
{
    auto& g = graphics;
    g.fillAll (juce::Colour (0xff171716));
    g.setColour (juce::Colour (0xff111110));
    g.fillRect (0, 0, 900, 640);

    // Satin aluminium enclosure and layered black pedal panels.
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffd0cec5), 256.0f, 8.0f,
                                             juce::Colour (0xff8f8e88), 644.0f, 632.0f, false));
    g.fillRoundedRectangle (256.0f, 8.0f, 388.0f, 624.0f, 12.0f);
    g.setColour (juce::Colour (0xffe1dfd7));
    g.drawRoundedRectangle (256.0f, 8.0f, 388.0f, 624.0f, 12.0f, 2.0f);

    g.setColour (juce::Colour (0xff090a0a));
    g.fillRoundedRectangle (263.0f, 15.0f, 374.0f, 205.0f, 22.0f);
    g.fillRoundedRectangle (263.0f, 228.0f, 374.0f, 263.0f, 24.0f);
    g.fillRoundedRectangle (263.0f, 499.0f, 374.0f, 123.0f, 22.0f);

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff40327c), 278.0f, 245.0f,
                                             juce::Colour (0xff211c50), 624.0f, 481.0f, false));
    g.fillRoundedRectangle (278.0f, 245.0f, 344.0f, 228.0f, 20.0f);
    g.setColour (juce::Colour (0xff7770a4));
    g.drawRoundedRectangle (278.0f, 245.0f, 344.0f, 228.0f, 20.0f, 1.0f);

    // Hardware labels and fine rate scale.
    g.setColour (cream);
    g.setFont (juce::FontOptions (16.0f, juce::Font::bold));
    g.drawText ("OUTPUT", 274, 28, 90, 24, juce::Justification::centredLeft);
    g.drawText ("9V", 433, 28, 40, 24, juce::Justification::centred);
    g.drawText ("INPUT", 535, 28, 88, 24, juce::Justification::centredRight);
    g.setFont (juce::FontOptions (17.0f, juce::Font::bold));
    g.drawText ("DEPTH", 271, 167, 92, 25, juce::Justification::centred);
    g.drawText ("RATE", 393, 185, 112, 27, juce::Justification::centred);
    g.drawText ("STATUS", 541, 167, 95, 25, juce::Justification::centred);

    const auto rateCentre = juce::Point<float> (450.0f, 117.0f);
    for (int tick = 0; tick < 15; ++tick)
    {
        const auto angle = juce::degreesToRadians (-132.0f + (264.0f * (float) tick / 14.0f));
        const auto inner = tick % 2 == 0 ? 87.0f : 91.0f;
        const auto outer = tick % 2 == 0 ? 101.0f : 98.0f;
        g.setColour (cream.withAlpha (0.92f));
        g.drawLine (rateCentre.x + std::sin (angle) * inner, rateCentre.y - std::cos (angle) * inner,
                    rateCentre.x + std::sin (angle) * outer, rateCentre.y - std::cos (angle) * outer,
                    tick % 2 == 0 ? 3.0f : 2.2f);
    }

    // Small Clone style label plate and footswitch surround.
    g.setColour (juce::Colour (0xffede9d8));
    g.setFont (juce::FontOptions (50.0f, juce::Font::bold));
    g.drawText ("SMALL", 303, 280, 294, 62, juce::Justification::centred);
    g.drawText ("CLONE", 303, 335, 294, 62, juce::Justification::centred);
    g.setFont (juce::FontOptions (20.0f));
    g.drawText ("EH 4600  FULL-CHORUS", 298, 392, 304, 32, juce::Justification::centred);

    g.setColour (juce::Colour (0xffdedbd0));
    g.setFont (juce::FontOptions (22.0f));
    g.drawText ("ELECTRO-HARMONIX", 278, 524, 344, 34, juce::Justification::centred);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText ("U.S.A.", 278, 559, 344, 22, juce::Justification::centred);

    // Status jewel lights only while the effect is engaged.
    const auto enabled = pluginProcessor.parameters.getRawParameterValue ("bypass")->load() < 0.5f;
    const auto ledCentre = juce::Point<float> (587.0f, 119.0f);
    g.setColour (juce::Colour (0xff292929));
    g.fillEllipse (ledCentre.x - 19.0f, ledCentre.y - 19.0f, 38.0f, 38.0f);
    g.setColour (juce::Colour (0xff72706d));
    g.drawEllipse (ledCentre.x - 16.0f, ledCentre.y - 16.0f, 32.0f, 32.0f, 2.0f);
    g.setColour (enabled ? juce::Colour (0xfff21e18) : juce::Colour (0xff52201e));
    g.fillEllipse (ledCentre.x - 11.0f, ledCentre.y - 11.0f, 22.0f, 22.0f);
    g.setColour (enabled ? juce::Colour (0xffff8a76) : juce::Colour (0xff75413c));
    g.fillEllipse (ledCentre.x - 6.0f, ledCentre.y - 8.0f, 8.0f, 5.0f);

    // Exposed plate screws.
    drawScrew (g, 280.0f, 28.0f);
    drawScrew (g, 620.0f, 28.0f);
    drawScrew (g, 280.0f, 205.0f);
    drawScrew (g, 620.0f, 205.0f);
    drawScrew (g, 280.0f, 482.0f);
    drawScrew (g, 620.0f, 482.0f);

}

void SmallCloneCircuitChorusAudioProcessorEditor::resized()
{
    rate.setBounds (376, 43, 148, 148);
    depth.setBounds (291, 83, 48, 70);
    bypass.setBounds (411, 420, 78, 72);
    sync.setBounds (704, 538, 160, 44);
}
