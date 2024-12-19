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
    juce::Label frequencyLabel;
    juce::Slider gainSlider;
	juce::Label gainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    OvertonePassFilterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OvertonePassFilterAudioProcessorEditor)
};