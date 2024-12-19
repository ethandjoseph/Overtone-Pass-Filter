#include "PluginProcessor.h"
#include "PluginEditor.h"

BandpassFilterAudioProcessorEditor::BandpassFilterAudioProcessorEditor (BandpassFilterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    frequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
	gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    qKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    qKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);

    frequencyLabel.setText("Note", juce::dontSendNotification);
    qLabel.setText("Resonance", juce::dontSendNotification);

    frequencyLabel.attachToComponent(&frequencySlider, false);
    qLabel.attachToComponent(&qKnob, false);

    addAndMakeVisible(frequencyLabel);
    addAndMakeVisible(qLabel);

	addAndMakeVisible(gainSlider);

    addAndMakeVisible(frequencySlider);
    addAndMakeVisible(qKnob);

    frequencySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "NOTE", frequencySlider);
	gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN_DB", gainSlider);
    qKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "R", qKnob);

    setSize (300, 300);
}

BandpassFilterAudioProcessorEditor::~BandpassFilterAudioProcessorEditor()
{
}

void BandpassFilterAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkolivegreen);
}

void BandpassFilterAudioProcessorEditor::resized()
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

    /*bounds.removeFromTop(gapBetweenSliderAndKnob);

    auto knobArea = bounds.removeFromTop(bounds.getHeight()).reduced(20);
    knobArea = knobArea.withTrimmedLeft((knobArea.getWidth() - knobWidth) / 2).withWidth(knobWidth);
    auto knobLabelArea = knobArea.removeFromTop(labelHeight);
    qLabel.setBounds(knobLabelArea);
    qKnob.setBounds(knobArea);*/
}