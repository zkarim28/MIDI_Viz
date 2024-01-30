#include "MainComponent.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <set>

//a very strange fix to my code implement the prevNotes data was to always check if the size of the vecotr
//is greater than 4.

//future improvemnts: make a slider to change size of keys
//color pallete for customizability

//white key width
int WHITE_KEY_SPACER = 18;

//black key width
int BLACK_KEY_SPACER = 12;

//used in moveNotes. controls how many pixels the y coordinate changed upward every refresh cycle
int NOTE_SPEED = 2;

//used for keyboard key information inside of displayKeyMap.
int KEY_HEIGHT = 100;

//constant num of MIDI signal possible.. from my keyboard at least
int NUM_KEYS = 121;

//juce::File currentWorkingDirectory = juce::File::getCurrentWorkingDirectory();
//juce::File imagePath = currentWorkingDirectory.getChildFile("Builds/MacOSX/bkg.jpeg"); // Relative path from the current working directory

/*THERE WAS A STRANGE FACTORING OF MY FILES WHEN I ADDED THE BKG IMG

 NOTICE:
 
 

BEFORE RUNNING THE PROGRAM, PLEASE EDIT THE backgroundImage directory
 string to the bkg.jpeg absolute path that works on your computer.
 otherwise the program will not be able to run

 juce::Image backgroundImage = juce::ImageFileFormat::loadFrom(juce::File("/...absolutePath/...bkg.jpeg"));
 
 1. find bkg somwhere in this folder, on xcode is can also be found in directly
 2. in xcode right click and click "Show File Inspector" or if not
in xcode, you can also find it in your folders anyway
 3. copy full path in and paste into the double quotes found within the juce::File() constructor inside of the loadFrom function.

*/

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

//for future use
//std::set<int> whiteKeys =
//    {0, 2, 4, 5, 7, 9, 11,        //C -2    0<noteID<12
//    12, 14, 16, 17, 19, 21, 23,   //C -1    <24
//    24, 26, 28, 29, 31, 33, 35,   //C 0     <36
//    36, 38, 40, 41, 43, 45, 47,   //C 1     <48
//    48, 50, 52, 53, 55, 57, 59,   //C 2     <60
//    60, 62, 64, 65, 67, 69, 71,   //C 3     <72
//    72, 74, 76, 77, 79, 81, 83,   //C 4     <84
//    84, 86, 88, 89, 91, 93, 95,   //C 5     <96
//    96, 98, 100, 101, 103, 105, 107,  //C 6   <108
//    108, 110, 112, 113, 115, 119, 120}; // C7 < 120

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

//    for (int i = 0; i < NUM_KEYS; i++) {
//        std::vector<int> key = {i*PIXEL_MULTIPLIER, 0, PIXEL_MULTIPLIER, KEY_HEIGHT, 0};
//        displayKeyMap.insert({i, key});
//    }
    
    int xCoordCounter = 0;
    
    for (int i = 0; i < NUM_KEYS; i++) {
        if (blackKeys.count(i) != 0){
            std::vector<int> key = {xCoordCounter, 0, BLACK_KEY_SPACER, KEY_HEIGHT, 0};
            displayKeyMap.insert({i, key});
            xCoordCounter += BLACK_KEY_SPACER;
        } else {
            std::vector<int> key = {xCoordCounter, 0, WHITE_KEY_SPACER, KEY_HEIGHT, 0};
            displayKeyMap.insert({i, key});
            xCoordCounter += WHITE_KEY_SPACER;
        }
    }
    setSize (xCoordCounter, backgroundImage.getHeight());
    setMidiInput();
    midiLabel.setText("midiText", juce::sendNotification);
    addAndMakeVisible(midiLabel);
    
    Timer::startTimerHz(120);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
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
//drawing occurs here. key and note data has already been processed beforehand
//now the notes are simply iterated through and utilized for graphics
//no change of data here
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
        g.setColour(juce::Colour(0, colorMultiplier*2, colorMultiplier*2));
        g.fillRect(note[0], note[1], note[2], note[3]);
        g.setColour(juce::Colours::black);
        g.drawRect(note[0], note[1], note[2], note[3], 1);
    }

    //display old notes
    for(std::vector<int>& note: prevNotes){
        if(note.size() >= 4){
            colorMultiplier = (note.back() < 10) ? 30 : note.back();
            g.setColour(juce::Colour(0, colorMultiplier*2, colorMultiplier*2));
            g.fillRect(note[0], note[1], note[2], note[3]);
            g.setColour(juce::Colours::black);
            g.drawRect(note[0], note[1], note[2], note[3], 1);
        }
    }
    
    //repaint displayKeys
    for (auto& [id, note]: displayKeyMap) {
        g.setColour((note.back() == 1) ? juce::Colours::blue : ((blackKeys.count(id)) ? juce::Colours::black : juce::Colours::grey));
        g.fillRect(note[0], Component::getHeight() - KEY_HEIGHT, note[2], note[3]);
        g.setColour((blackKeys.count(id)) ? juce::Colours::grey : juce::Colours::black);
        g.drawRect(note[0], Component::getHeight() - KEY_HEIGHT, note[2], note[3], 2);
    }
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    midiLabel.setBounds(getLocalBounds().withTop(0));
}

//==============================================================================

//NOTE: KEYBOARD MUST BE PLUGGED IN BEFORE STARTING PROGRAM
//finds all midi inputs, and selects the first to be opened and added to
//device call back for interaction in program
void MainComponent::setMidiInput()
{
    auto list = juce::MidiInput::getAvailableDevices();

    auto newInput = list[0];

    if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

    deviceManager.addMidiInputDeviceCallback (newInput.identifier, this);
}


//is handled on a separate thread
//as soon as a key is played or released, that data is processed here
// if on, added to activeNotes set
// if off, copied and deleted form activeNotes and inserted into prevNotes


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
    int noteWidth = (blackKeys.count(noteID)) ? BLACK_KEY_SPACER : WHITE_KEY_SPACER;
    
    if (velocity > 0) {
        int keyXCoord = displayKeyMap[noteID].at(0);
        std::vector<int> note = {keyXCoord, Component::getHeight()-KEY_HEIGHT, noteWidth, 1, isBlackKey, velocity};
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

//straming note coordinates are editted by subtracting y coord as xy-coord
//system has the positive direction go down
//strewaming notes move upward
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

//==============================================================================
//this is the thread that repeatedly edits streaming notes coordinates
//then repaints them immediately after to keep the program continuous
void MainComponent::timerCallback()
{
    moveNotes();
    repaint();
}
