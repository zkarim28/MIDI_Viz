#include "MainComponent.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <set>

//#include "juce_MidiKeyboardComponent.h"
//test change for github

std::unordered_map<int, juce::Rectangle<int>> activeNotes;
std::vector<juce::Rectangle<int>> prevNotes (120);
std::set<int> whiteKeys = {2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31, 33, 35, 36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88, 89, 91, 93, 95, 96, 98, 100, 101, 103, 105, 107, 108, 110, 112, 113, 115, 119, 120};
std::set<int> blackKeys = {1, 3, 6, 8, 10, 13, 15, 18, 20, 22, 25, 27, 30, 32, 34, 37, 39, 42, 44, 46, 49, 51, 54, 56, 58, 61, 63, 66, 68, 70, 73, 75, 78, 80, 82, 85, 87, 90, 92, 94, 97, 99, 102, 104, 106, 109, 111, 114, 116, 118};
int PIXEL_MULTIPLIER = 7;
int NOTE_SPEED = 2;
juce::Image backgroundImage = juce::ImageFileFormat::loadFrom(juce::File("/Users/zarifkarim/Documents/MIDI_Viz/Builds/MacOSX/bkg.jpeg"));


//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
    //another possible idea is to have a button that lets you select if you have either 88 keys or 120 possible keys
    
    setSize (1000, 600);
    
    setMidiInput();
    midiLabel.setText("midiText", juce::sendNotification);
    addAndMakeVisible(midiLabel);
    Timer::startTimerHz(120);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
//    updatingThread.stopThread(1000);
    Timer::stopTimer();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{

}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    
    /* Here is where we draw the notes with the given MIDI note information.
     Iterate through activeNotes hashmap and prevNotes vector and simply
     draw wth info. other function handle change of data, specifically height and
     y coordinate.
     */
    
    // You can add your drawing code
    
    g.drawImageWithin(backgroundImage, 0, 0, Component::getWidth(), Component::getHeight(), juce::RectanglePlacement::fillDestination);
    g.setColour(juce::Colours::orange);
    g.fillRect(10, 10, 40, 40);
    
    for (auto& [id, note]: activeNotes) {
        g.setColour(juce::Colours::orange);
        g.fillRect(note.getX() * PIXEL_MULTIPLIER , note.getY(), note.getWidth(), note.getHeight());
    }
    
    for (int i = 0; i < prevNotes.size(); i++){
        g.setColour(juce::Colours::orange);
        g.fillRect(prevNotes.at(i).getX() * PIXEL_MULTIPLIER , prevNotes.at(i).getY() , prevNotes.at(i).getWidth(), prevNotes.at(i).getHeight());
    }
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

//    comp.setBounds(getLocalBounds().withSizeKeepingCentre(400, 300));
//    midiLabel.setBounds(getLocalBounds().withSizeKeepingCentre(400, 300));
    midiLabel.setBounds(getLocalBounds().withTop(0));
}

void MainComponent::handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage& message)
{
    DBG("Received MIDI message: " << message.getDescription());
    
    //display label test
    juce::String midiText;
    if (message.isNoteOnOrOff()) {
        midiText << "NoteOn: Channel " << message.getChannel();
        midiText << ":Number" << message.getNoteNumber();
        midiText << ":Velocity" << message.getVelocity();
    }
    midiLabel.getTextValue() = midiText;
    
    int noteID = message.getNoteNumber();
    int velocity = message.getVelocity();
    bool isBlackKey = blackKeys.count(noteID) == 1;
    int noteWidth = (isBlackKey) ? 4 : 6;
//    int xOffset = (isBlackKey) ? 1 : 0;
    if (velocity > 0) {
        juce::Rectangle<int> note (noteID, Component::getHeight(), noteWidth, 1);
        activeNotes.insert({noteID, note});
    } else {
        if (activeNotes.count(noteID)==1){
            prevNotes.push_back(activeNotes[noteID]); activeNotes[noteID];
            activeNotes.erase(noteID);
        }
    }
}

void MainComponent::setMidiInput()
{
    auto list = juce::MidiInput::getAvailableDevices();

    auto newInput = list[0];

    if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

    deviceManager.addMidiInputDeviceCallback (newInput.identifier, this);

}

void MainComponent::moveNotes()
{
    for (auto& [noteID, note] : activeNotes){
        juce::Rectangle<int> updatedNote (activeNotes[noteID].getX(), activeNotes[noteID].getY()-NOTE_SPEED, activeNotes[noteID].getWidth(), activeNotes[noteID].getHeight()+NOTE_SPEED);
        activeNotes[noteID] = updatedNote;
    }
    
    for (int i = 0; i < prevNotes.size(); i++) {
//        if (prevNotes.at(i).getY() + prevNotes.at(i).getHeight() < 0){
//            prevNotes.erase(prevNotes.begin()+i);
//            i--;
//        }
        juce::Rectangle<int> updatedNote (prevNotes.at(i).getX(), prevNotes.at(i).getY()-NOTE_SPEED, prevNotes.at(i).getWidth(), prevNotes.at(i).getHeight());
        prevNotes[i] = updatedNote;
    }
}

void MainComponent::timerCallback()
{
    moveNotes();
    repaint();
}
