#include "PluginProcessor.h"
#include "PluginEditor.h"

BandpassFilterAudioProcessor::BandpassFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

BandpassFilterAudioProcessor::~BandpassFilterAudioProcessor()
{
}

const juce::String BandpassFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BandpassFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BandpassFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BandpassFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BandpassFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BandpassFilterAudioProcessor::getNumPrograms()
{
    return 1;
}

int BandpassFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BandpassFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BandpassFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void BandpassFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void BandpassFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    bandPassFilter1.prepare(spec);
    bandPassFilter1.reset();
    bandPassFilter1.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

    bandPassFilter2.prepare(spec);
    bandPassFilter2.reset();
    bandPassFilter2.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

    bandPassFilter3.prepare(spec);
    bandPassFilter3.reset();
    bandPassFilter3.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

    bandPassFilter4.prepare(spec);
    bandPassFilter4.reset();
    bandPassFilter4.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
}

void BandpassFilterAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BandpassFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// Process Block ==============================================================================
void BandpassFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float note = apvts.getRawParameterValue("NOTE")->load();
    //float frequency = apvts.getRawParameterValue("FREQUENCY")->load();
	float frequency = juce::MidiMessage::getMidiNoteInHertz(note);
    float resonance = apvts.getRawParameterValue("R")->load();
    float gainValue = apvts.getRawParameterValue("GAIN_DB")->load();
    float linearGain = juce::Decibels::decibelsToGain (-18.0);//(gainValue);

    bandPassFilter1.setCutoffFrequency(frequency);
    bandPassFilter2.setCutoffFrequency(2*frequency);
    bandPassFilter3.setCutoffFrequency(4*frequency);
    bandPassFilter4.setCutoffFrequency(8*frequency);
    bandPassFilter1.setResonance(8.0);//(resonance);
    bandPassFilter2.setResonance(8.0);//(resonance);
    bandPassFilter3.setResonance(8.0);//(resonance);
    bandPassFilter4.setResonance(8.0);//(resonance);

    juce::AudioBuffer<float> tempBuffer1, tempBuffer2, tempBuffer3, tempBuffer4;

    tempBuffer1.makeCopyOf(buffer);
    tempBuffer2.makeCopyOf(buffer);
    tempBuffer3.makeCopyOf(buffer);
    tempBuffer4.makeCopyOf(buffer);

    auto audioBlock1 = juce::dsp::AudioBlock<float>(tempBuffer1);
    auto audioBlock2 = juce::dsp::AudioBlock<float>(tempBuffer2);
    auto audioBlock3 = juce::dsp::AudioBlock<float>(tempBuffer3);
    auto audioBlock4 = juce::dsp::AudioBlock<float>(tempBuffer4);

    auto context1 = juce::dsp::ProcessContextReplacing<float>(audioBlock1);
    auto context2 = juce::dsp::ProcessContextReplacing<float>(audioBlock2);
    auto context3 = juce::dsp::ProcessContextReplacing<float>(audioBlock3);
    auto context4 = juce::dsp::ProcessContextReplacing<float>(audioBlock4);

    bandPassFilter1.process(context1);
    bandPassFilter2.process(context2);
    bandPassFilter3.process(context3);
    bandPassFilter4.process(context4);

    /*auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);
    bandPassFilter.process(context);*/

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* mainBuffer = buffer.getWritePointer(channel);
        auto* data1 = tempBuffer1.getReadPointer(channel);
        auto* data2 = tempBuffer2.getReadPointer(channel);
        auto* data3 = tempBuffer3.getReadPointer(channel);
        auto* data4 = tempBuffer4.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            mainBuffer[sample] = data1[sample] + data2[sample] + data3[sample] + data4[sample];
        }
    }

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= linearGain;
        }
    }
    
}
//==============================================================================

bool BandpassFilterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BandpassFilterAudioProcessor::createEditor()
{
    return new BandpassFilterAudioProcessorEditor (*this);
}

void BandpassFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void BandpassFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BandpassFilterAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "FREQUENCY",               // Parameter ID
        "Centre Frequency",        // Parameter name
        juce::NormalisableRange{
                    20.f,          // rangeStart
                    20000.f,       // rangeEnd
                    0.1f,          // intervalValue
                    0.2f,          // skewFactor
                    false },       // useSymmetricSkew
                    -12.0f         // Default value
                    ));

	params.push_back(std::make_unique<juce::AudioParameterFloat>(
		"NOTE",                // Parameter ID
		"Note",                // Parameter name
        juce::NormalisableRange<float>(0.0f, 127.0f, 1.0f),
		60                     // Default value
	    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "R",                // Parameter ID
        "Resonance",        // Parameter name
        0.707f,             // minValue
        8.000f,             // maxValue
        0.707f              // Default value
        ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "GAIN_DB",                // Parameter ID
        "Gain in dB",        // Parameter name
        -60.f,             // minValue
        12.f,             // maxValue
        0.f              // Default value
        ));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BandpassFilterAudioProcessor();
}