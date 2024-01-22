#pragma once

#include <JuceHeader.h>
#include <unordered_map>
#include <list>
#include <vector>



//class DelayedOneShotLambda : public juce::Timer
//{
//public:
//    DelayedOneShotLambda(int ms, std::function<void()> fn) : 
//        func(fn)
//    {
//        startTimer(ms);
//    }
//    ~DelayedOneShotLambda() { stopTimer(); }
//    
//    void timerCallback() override
//    {
//        auto f = func;
//        delete this;
//        f();
//    }
//private:
//    std::function<void()> func;
//};
//
//
//struct AnimatedComponent : public juce::Component
//{
//    void paint (juce::Graphics& g) override;
//    
//    void mouseEnter(const juce::MouseEvent& e) override;
//    void mouseExit(const juce::MouseEvent& e) override;
//    
//    juce::Colour bkgcolr{0xFFFF0000};
//    
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
//    void moveNotes();
    void timerCallback() override;
    
private:
    //==============================================================================
    // Your private member variables go here...
//    AnimatedComponent comp;
    
    juce::AudioDeviceManager audioDeviceManager;
    juce::Label midiLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    
//    class UpdatingThread : public juce::Thread, public juce::Component
//    {
//    public :
//        UpdatingThread() : juce::Thread("Updating Thread")
//        {
//            
//        }
//        
//        void run() override
//        {
//            
//            const juce::MessageManagerLock mmLock;
//            
//            moveNotes(activeNotes, prevNotes, NOTE_SPEED);
//            repaint();
//            signalThreadShouldExit();
//        }
//        
//        void moveNotes(std::unordered_map<int, juce::Rectangle<int>> activeNotes, std::vector<juce::Rectangle<int>> prevNotes, int NOTE_SPEED)
//        {
//            for (auto& [noteID, note] : activeNotes){
//                juce::Rectangle<int> updatedNote (activeNotes[noteID].getX(), activeNotes[noteID].getY()-NOTE_SPEED, activeNotes[noteID].getWidth(), activeNotes[noteID].getHeight()+NOTE_SPEED);
//                activeNotes[noteID] = updatedNote;
//            }
//            
//            for (int i = 0; i < prevNotes.size(); i++) {
//                juce::Rectangle<int> updatedNote (prevNotes.at(i).getX(), prevNotes.at(i).getY()-NOTE_SPEED, prevNotes.at(i).getWidth(), prevNotes.at(i).getHeight());
//                prevNotes[i] = updatedNote;
//            }
//        }
//        
//        std::unordered_map<int, juce::Rectangle<int>> activeNotes;
//        std::vector<juce::Rectangle<int>> prevNotes;
//        int NOTE_SPEED;
//        
////        void paint (juce::Graphics& g) override;
////        void repaint ();
//    };
//    
//    UpdatingThread updatingThread;
};
