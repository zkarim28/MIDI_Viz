#include "MainComponent.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <set>

//a very strange fix to my code implement the prevNotes data was to always check if the size of the vecotr
//is greater than 4.

//is multiplied with the noteID to allow for horizontal separation of displayed notes x coordinates
int PIXEL_MULTIPLIER = 15;

//used in moveNotes. controls how many pixels the y coordinate changed upward every refresh cycle
int NOTE_SPEED = 2;

//used for keyboard key information inside of displayKeyMap.
int KEY_HEIGHT = 85;

//constant num of MIDI signal possible.. from my keyboard at least
int NUM_KEYS = 121;

juce::Image backgroundImage = juce::ImageFileFormat::loadFrom(juce::File("/Users/zarifkarim/Documents/MIDI_Viz/Builds/MacOSX/bkg.jpeg"));

//activNotes tracks all currently held down keys. Since there is only one possibility of each note,
//it is easy to access each note separately via a hashmap
//data layout: {..., {,{}} , {noteID, {x, y, width, height, isBlack, velocity}} , {,{}} , ...}
std::unordered_map<int ,std::vector<int>> activeNotes;

//prevNotes tracks all previously played notes, preserving all note information from activeNotes.
//Since there can be multiple previous notes played from the same key, having a nested vector list allows for duplicates of noteIDs
//yet with differing note information
//data layout: {..., {}, {noteID, x, y, width, height, isBlack, velocity}, {}, ...}
std::vector<std::vector<int>> prevNotes;

//displayKeyMap represents the keys on the keyboard. keys on screen change color accoring to what key is played
//and return to origianl color when not played.
//Since there is only one of each key, it is easier to access the key information through a hashmap
std::unordered_map<int, std::vector<int>> displayKeyMap;
//{x, y, width, height, isOn}

std::set<int> whiteKeys = 
    {0, 2, 4, 5, 7, 9, 11,        //C -2    0<noteID<12
    12, 14, 16, 17, 19, 21, 23,   //C -1    <24
    24, 26, 28, 29, 31, 33, 35,   //C 0     <36
    36, 38, 40, 41, 43, 45, 47,   //C 1     <48
    48, 50, 52, 53, 55, 57, 59,   //C 2     <60
    60, 62, 64, 65, 67, 69, 71,   //C 3     <72
    72, 74, 76, 77, 79, 81, 83,   //C 4     <84
    84, 86, 88, 89, 91, 93, 95,   //C 5     <96
    96, 98, 100, 101, 103, 105, 107,  //C 6   <108
    108, 110, 112, 113, 115, 119, 120}; // C7 < 120

//list of all MIDI NoteIDs that are linked to the black keys on any MIDI keyboard
std::set<int> blackKeys = {1, 3, 6, 8, 10, 13, 15, 18, 20, 22, 25, 27, 30, 32, 34, 37, 39, 42, 44, 46, 49, 51, 54, 56, 58, 61, 63, 66, 68, 70, 73, 75, 78, 80, 82, 85, 87, 90, 92, 94, 97, 99, 102, 104, 106, 109, 111, 114, 116, 118};

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
    
    setSize (NUM_KEYS * PIXEL_MULTIPLIER, backgroundImage.getHeight());

    for (int i = 0; i < NUM_KEYS; i++) {
        std::vector<int> key = {i*PIXEL_MULTIPLIER, 0, PIXEL_MULTIPLIER, KEY_HEIGHT, 0};
        displayKeyMap.insert({i, key});
    }
    
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
{}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    
    /* Here is where we draw the notes with the given MIDI note information.
     Iterate through activeNotes hashmap and prevNotes vector and simply
     draw wth info. other function handle change of data, specifically height and
     y coordinate.
     */
    
    //draw background image
    g.drawImageWithin(backgroundImage, 0, 0, Component::getWidth(), Component::getHeight(), juce::RectanglePlacement::fillDestination);
    
    int colorMultiplier = 0;
    
    //display new notes
    for (auto& [id, note]: activeNotes) {
        colorMultiplier = (note.back() < 10) ? 30 : note.back();
//        g.setColour(juce::Colour(0, note.back()*2, note.back()*2));
        g.setColour(juce::Colour(0, colorMultiplier*2, colorMultiplier*2));
//        g.fillRect(note.getX(), note.getY(), note.getWidth(), note.getHeight());
        g.fillRect(note[0], note[1], note[2], note[3]);
        g.setColour(juce::Colours::black);
//        g.drawRect(note.getX(), note.getY(), note.getWidth(), note.getHeight(), 1);
        g.drawRect(note[0], note[1], note[2], note[3], 1);
    }

    //display old notes
    for(std::vector<int>& note: prevNotes){
        if(note.size() >= 4){
//            g.setColour(juce::Colour(0, note.back()*2, note.back()*2));
            colorMultiplier = (note.back() < 10) ? 30 : note.back();
            g.setColour(juce::Colour(0, colorMultiplier*2, colorMultiplier*2));
            g.fillRect(note[0], note[1], note[2], note[3]);
            g.setColour(juce::Colours::black);
            g.drawRect(note[0], note[1], note[2], note[3], 1);
        }
        //        g.fillRect(prevNotes.at(i).getX(), prevNotes.at(i).getY() , prevNotes.at(i).getWidth(), prevNotes.at(i).getHeight());
        //        g.fillRect(prevNotes[i].at(0), prevNotes[i].at(1), prevNotes[i].at(2), prevNotes[i].at(3));
        //        g.drawRect(prevNotes.at(i).getX(), prevNotes.at(i).getY() , prevNotes.at(i).getWidth(), prevNotes.at(i).getHeight(), 1);
        //        g.drawRect(prevNotes[i].at(0), prevNotes[i].at(1), prevNotes[i].at(2), prevNotes[i].at(3));
    }
    
    //repaint displayKeys
    for (auto& [id, note]: displayKeyMap) {
        g.setColour((note.back() == 1) ? juce::Colours::blue : ((blackKeys.count(id)) ? juce::Colours::black : juce::Colours::grey));
        g.fillRect(note[0], Component::getHeight() - KEY_HEIGHT, note[2], note[3]);
        g.setColour((blackKeys.count(id)) ? juce::Colours::grey : juce::Colours::black);
        g.drawRect(note[0], Component::getHeight() - KEY_HEIGHT, note[2], note[3], 3);
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
    juce::String midiText;
    if (message.isNoteOnOrOff()) {
        midiText << "NoteOn: Channel " << message.getChannel();
        midiText << ":Number" << message.getNoteNumber();
        midiText << ":Velocity" << message.getVelocity();
    }
    midiLabel.getTextValue() = midiText;
    
    int noteID = message.getNoteNumber();
    int velocity = message.getVelocity();
    bool isBlackKey = blackKeys.count(noteID) != 0;
    int noteWidth = PIXEL_MULTIPLIER;
    if (velocity > 0) {
        std::vector<int> note = {noteID*PIXEL_MULTIPLIER, Component::getHeight()-KEY_HEIGHT, noteWidth, 1, isBlackKey, velocity};
        displayKeyMap[noteID].back() = 1;
        activeNotes.insert({noteID, note});
    } else {
        if (activeNotes.count(noteID)==1){
            prevNotes.push_back(activeNotes[noteID]);
            displayKeyMap[noteID].back() = 0;
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
        std::vector<int> updatedNote = {note.at(0), note.at(1)-NOTE_SPEED, note.at(2), note.at(3)+NOTE_SPEED, note.at(4), note.at(5)};
        activeNotes[noteID] = updatedNote;
    }
    
    for(size_t i = 0; i < prevNotes.size(); i++){
        std::vector<int>& note = prevNotes[i];
        if (note.size() >= 4 && note[1] + note[3] < 0){
            prevNotes.erase(prevNotes.begin()+i);
        }
        if(note.size() >= 4){
            std::vector<int> updatedNote = {note[0], note[1]-NOTE_SPEED, note[2], note[3], note[4], note[5]};
            prevNotes[i] = updatedNote;
        }
    }
}

void MainComponent::timerCallback()
{
    moveNotes();
    repaint();
}
