#include "MainComponent.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <vector>

//#include "juce_MidiKeyboardComponent.h"
//test change for github

std::unordered_map<int, juce::Rectangle<int>> activeNotes;
//std::list<juce::Rectangle<int>> prevNotes;
std::vector<juce::Rectangle<int>> prevNotes (120);
int PIXEL_MULTIPLIER = 7;
int NOTE_SPEED = 2;

void AnimatedComponent::paint(juce::Graphics &g)
{
//    g.fillAll(juce::Colours::white);
    g.fillAll(bkgcolr);
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void AnimatedComponent::mouseExit(const juce::MouseEvent &e){
    bkgcolr = juce::Colour(0xFFFF0000);
    repaint();
}

void AnimatedComponent::mouseEnter(const juce::MouseEvent &e){
    bkgcolr = juce::Colour(0xFFFF00FF);
    
    new DelayedOneShotLambda(1000, [this]()
                             { DBG( "bounds: " + getLocalBounds().toString() ); }
                             );
    
    repaint();
}



//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
//    addAndMakeVisible(comp);
    setSize (800, 600);
    setMidiInput();
    
    //midiLabel
    midiLabel.setText("midiText", juce::sendNotification);
    addAndMakeVisible(midiLabel);
    
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
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    
    /* Here is where we draw the notes with the given MIDI note information.
     Iterate through activeNotes hashmap and prevNotes vector and simply
     draw wth info. other function handle change of data, specifically height and
     y coordinate.
     */
    
    moveNotes();
    
    
    
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    // You can add your drawing code here!
    //    juce::Rectangle<int> area (10, 10, 40, 40);
    
    //for (Rectangle in Rectangle list){
    //
    //
    //}
    
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
//    g.fillRect(area);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    comp.setBounds(getLocalBounds().withSizeKeepingCentre(400, 300));
    midiLabel.setBounds(getLocalBounds().withSizeKeepingCentre(400, 300));
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
    DBG(midiText);
    
    //it seems that all code will be going here, just like in Java edition
    //ToDo: make global variables to use to store all midi messages
    //toDo: Find a way to draw a note similar to how i did in Java edition
    
    int noteID = message.getNoteNumber();
    int velocity = message.getVelocity();
    if (velocity > 0) {
        juce::Rectangle<int> note (noteID, 300, 5, 10);
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
        juce::Rectangle<int> updatedNote (prevNotes.at(i).getX(), prevNotes.at(i).getY()-NOTE_SPEED, prevNotes.at(i).getWidth(), prevNotes.at(i).getHeight());
        prevNotes[i] = updatedNote;
    }
}
