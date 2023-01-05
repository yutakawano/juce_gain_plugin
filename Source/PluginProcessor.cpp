/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainPluginAudioProcessor::GainPluginAudioProcessor()
     : parameters(*this, nullptr, juce::Identifier("APVTSTutorial"),
                  {
         std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0f, 1.0f, 0.5f),
         std::make_unique<juce::AudioParameterBool>("invertPhase", "Invert Phase", false)
     })
{
    phaseParameter = parameters.getRawParameterValue("invertPhase");
    gainParameter = parameters.getRawParameterValue("gain");
}

GainPluginAudioProcessor::~GainPluginAudioProcessor()
{
}

//==============================================================================
const juce::String GainPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GainPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GainPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GainPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GainPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GainPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void GainPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GainPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // auto phase = *invertPhase ? -1.0f : 1.0f;
    // previousGain = *gain * phase;
    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    previousGain = *gainParameter * phase;
}

void GainPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GainPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // auto phase = *invertPhase ? -1.0f : 1.0f;
    // auto currentGain = *gain * phase;
    
    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    auto currentGain = *gainParameter * phase;
    
    if (currentGain == previousGain)
    {
        buffer.applyGain(currentGain);
    }
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousGain, currentGain);
        previousGain = currentGain;
    }
}

//==============================================================================
bool GainPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GainPluginAudioProcessor::createEditor()
{
    // return new GainPluginAudioProcessorEditor (*this);
    
    return new GainPluginAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void GainPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("ParamTutorial"));
    // xml->setAttribute("gain", (double) *gain);
    // xml->setAttribute("invertPhase", *invertPhase);
    
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GainPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
//    if (xmlState.get() != nullptr)
//        if (xmlState->hasTagName("ParamTutorial"))
//            *gain = (float) xmlState->getDoubleAttribute("gain", 1.0);
//            *invertPhase = xmlState->getBoolAttribute("invertPhase", false);
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainPluginAudioProcessor();
}
