#pragma once

#include <JuceHeader.h>
#include <unordered_map>
#include <list>
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::MidiInputCallback, public::juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;
    
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    //=============================================================================
    
    void handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage&) override;
    void setMidiInput();
    void moveNotes();
    void timerCallback() override;
    
private:
    //==============================================================================
    // Your private member variables go here...
    //    AnimatedComponent comp;
    
    juce::AudioDeviceManager audioDeviceManager;
    juce::Label midiLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent);
};
