#pragma once

#include <JuceHeader.h>
#include <unordered_map>
#include <list>
#include <vector>

//struct Note
//{
//public:
//    Note(int x, int y, int width, int height, int velocity, bool whiteOrBlack)
//    {
//        this->x = x;
//        this->y = y;
//        this->width = width;
//        this->height = height;
//        this->velocity = velocity;
//        this->whiteOrBlack = whiteOrBlack;
//    };
//    
//    int getX() {return x;};
//    int getY() {return y;};
//    int getWidth() {return width;};
//    int getHeight() {return height;};
//    int getVelocity() {return velocity;};
//    bool getWhiteOrBlack() {return whiteOrBlack;};
//    
//private:
//    int x;
//    int y;
//    int height;
//    int width;
//    int velocity;
//    bool whiteOrBlack;
//};
//
//struct Key
//{
//public:
//    Note(int x, int y, int width, int height, bool whiteOrBlack)
//    {
//        this->x = x;
//        this->y = y;
//        this->width = width;
//        this->height = height;
//        this->whiteOrBlack = whiteOrBlack;
//    };
//    
//    int getX() {return x;};
//    int getY() {return y;};
//    int getWidth() {return width;};
//    int getHeight() {return height;};
//    bool getWhiteOrBlack() {return whiteOrBlack;};
//    
//private:
//    int x;
//    int y;
//    int width;
//    int height;
//    bool whiteOrBlack;
//};


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
    
    juce::AudioDeviceManager audioDeviceManager;
    juce::Label midiLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent);
};
