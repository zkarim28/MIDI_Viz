#pragma once

#include <JuceHeader.h>



class DelayedOneShotLambda : public juce::Timer
{
public:
    DelayedOneShotLambda(int ms, std::function<void()> fn) : 
        func(fn)
    {
        startTimer(ms);
    }
    ~DelayedOneShotLambda() { stopTimer(); }
    
    void timerCallback() override
    {
        auto f = func;
        delete this;
        f();
    }
private:
    std::function<void()> func;
};


struct AnimatedComponent : public juce::Component
{
    void paint (juce::Graphics& g) override;
    
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    
    juce::Colour bkgcolr{0xFFFF0000};
    
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::MidiInputCallback
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
    
    void handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage&);
    void setMidiInput();
    
private:
    //==============================================================================
    // Your private member variables go here...
    AnimatedComponent comp;
    
    juce::AudioDeviceManager audioDeviceManager;
    juce::Label midiLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
