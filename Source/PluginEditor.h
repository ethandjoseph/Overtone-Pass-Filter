#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class OvertonePassFilterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OvertonePassFilterAudioProcessorEditor (OvertonePassFilterAudioProcessor&);
    ~OvertonePassFilterAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider frequencySlider;
    juce::Slider qKnob;
    juce::Label frequencyLabel;
    juce::Label qLabel;
    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qKnobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    OvertonePassFilterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OvertonePassFilterAudioProcessorEditor)
};