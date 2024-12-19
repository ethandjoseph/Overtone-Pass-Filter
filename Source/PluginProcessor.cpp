#include "PluginProcessor.h"
#include "PluginEditor.h"

OvertonePassFilterAudioProcessor::OvertonePassFilterAudioProcessor()
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

OvertonePassFilterAudioProcessor::~OvertonePassFilterAudioProcessor()
{
}

const juce::String OvertonePassFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OvertonePassFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OvertonePassFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OvertonePassFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OvertonePassFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OvertonePassFilterAudioProcessor::getNumPrograms()
{
    return 1;
}

int OvertonePassFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OvertonePassFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OvertonePassFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void OvertonePassFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OvertonePassFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
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

void OvertonePassFilterAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OvertonePassFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
void OvertonePassFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float note = apvts.getRawParameterValue("NOTE")->load();
    float frequency = juce::MidiMessage::getMidiNoteInHertz(note);
    float gainValue = apvts.getRawParameterValue("GAIN_DB")->load();
    float linearGain = juce::Decibels::decibelsToGain(gainValue);

    //===========================================================================
    juce::AudioBuffer<float> tempBuffer1, tempBuffer2, tempBuffer3, tempBuffer4;

    tempBuffer1.makeCopyOf(buffer);
    tempBuffer2.makeCopyOf(buffer);
    tempBuffer3.makeCopyOf(buffer);
    tempBuffer4.makeCopyOf(buffer);
    //===========================================================================


	bool filterBypass1, filterBypass2, filterBypass3, filterBypass4 = false;

	if ((8*frequency) > 20000.)
	{
		filterBypass4 = true;
		tempBuffer4.clear();
		bandPassFilter4.reset();
	}
    else
    {
        filterBypass4 = false;
    }

	if ((4*frequency) > 20000.)
	{
		filterBypass3 = true;
        tempBuffer3.clear();
		bandPassFilter3.reset();
	}
    else
	{
		filterBypass3 = false;
	}

	if ((2*frequency) > 20000.)
	{
		filterBypass2 = true;
        tempBuffer2.clear();
		bandPassFilter2.reset();
	}
	else
	{
		filterBypass2 = false;
	}

	if (frequency > 20000.)
	{
		filterBypass1 = true;
        tempBuffer1.clear();
		bandPassFilter1.reset();
	}
	else
	{
		filterBypass1 = false;
	}

	if (!filterBypass1)
	{
		bandPassFilter1.setCutoffFrequency(frequency);
		bandPassFilter1.setResonance(8.0);//(resonance);
	}

	if (!filterBypass2)
	{
        bandPassFilter2.setCutoffFrequency(2 * frequency);
        bandPassFilter2.setResonance(8.0);//(resonance);

	}
    
	if (!filterBypass3)
	{
		bandPassFilter3.setCutoffFrequency(4 * frequency);
        bandPassFilter3.setResonance(8.0);//(resonance);

	}

    if (!filterBypass4)
    {
        bandPassFilter4.setCutoffFrequency(8 * frequency);
        bandPassFilter4.setResonance(8.0);//(resonance);
    }

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

	float filterGain1 = juce::Decibels::decibelsToGain(0.);
	float filterGain2 = juce::Decibels::decibelsToGain(-2.);
	float filterGain3 = juce::Decibels::decibelsToGain(-4.);
	float filterGain4 = juce::Decibels::decibelsToGain(-6.);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* mainBuffer = buffer.getWritePointer(channel);
        auto* data1 = tempBuffer1.getReadPointer(channel);
        auto* data2 = tempBuffer2.getReadPointer(channel);
        auto* data3 = tempBuffer3.getReadPointer(channel);
        auto* data4 = tempBuffer4.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            mainBuffer[sample] = data1[sample] + filterGain2*data2[sample] + filterGain3*data3[sample] + filterGain4*data4[sample];
        }
    }

    float gainCompensation = juce::Decibels::decibelsToGain(-18.0);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= gainCompensation;
            channelData[sample] *= linearGain;
        }
    }
}
//==============================================================================

bool OvertonePassFilterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* OvertonePassFilterAudioProcessor::createEditor()
{
    return new OvertonePassFilterAudioProcessorEditor (*this);
}

void OvertonePassFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void OvertonePassFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessorValueTreeState::ParameterLayout OvertonePassFilterAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "NOTE",                // Parameter ID
        "Note",                // Parameter name
        juce::NormalisableRange<float>(0.0f, 127.0f, 1.0f),
        60                     // Default value
        ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "GAIN_DB",             // Parameter ID
        "Gain in dB",          // Parameter name
        -60.f,                 // minValue
        6.f,                   // maxValue
        0.f                    // Default value
        ));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OvertonePassFilterAudioProcessor();
}