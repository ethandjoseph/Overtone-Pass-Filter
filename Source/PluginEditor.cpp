#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MidiNote.h"

OvertonePassFilterAudioProcessorEditor::OvertonePassFilterAudioProcessorEditor (OvertonePassFilterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    frequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);

    frequencyLabel.setText("Note", juce::dontSendNotification);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);

    addAndMakeVisible(frequencySlider);
    addAndMakeVisible(frequencyLabel);
    addAndMakeVisible(gainSlider);
	addAndMakeVisible(gainLabel);

    frequencySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "NOTE", frequencySlider);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN_DB", gainSlider);

    frequencySlider.textFromValueFunction = [](double value)
        {
            int midiNoteNumber = static_cast<int>(value);
            //DBG("Slider Value: " + juce::String(value));
            juce::String midiNoteName = getMidiNoteName(midiNoteNumber);
            //DBG("MIDI Note: " + midiNoteName);
            return midiNoteName;
        };

    setSize(300, 300);
}

OvertonePassFilterAudioProcessorEditor::~OvertonePassFilterAudioProcessorEditor()
{
}

void OvertonePassFilterAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::deeppink);
}

void OvertonePassFilterAudioProcessorEditor::resized()
{
    const int padding = 10;
    const int labelHeight = 20;
    const int gapBetweenSliderAndKnob = 20;

    auto bounds = getLocalBounds().reduced(10);

    const int sliderWidth = bounds.getWidth() * 0.75f;
    const int knobWidth = bounds.getWidth() * 0.5f;

    auto sliderArea = bounds.removeFromTop(bounds.getHeight() * 0.3f - gapBetweenSliderAndKnob / 2);
    auto sliderLabelArea = sliderArea.removeFromTop(labelHeight);
    frequencyLabel.setBounds(sliderLabelArea);
    frequencySlider.setBounds(sliderArea);

    bounds.removeFromTop(gapBetweenSliderAndKnob);

    auto knobArea = bounds.removeFromTop(bounds.getHeight()).reduced(20);
    knobArea = knobArea.withTrimmedLeft((knobArea.getWidth() - knobWidth) / 2).withWidth(knobWidth);
    auto knobLabelArea = knobArea.removeFromTop(labelHeight);
    gainLabel.setBounds(knobLabelArea);
    gainSlider.setBounds(knobArea);
}