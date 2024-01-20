#include "MainComponent.h"
#include <iostream>
#include <string>

//#include "juce_MidiKeyboardComponent.h"
//test change for github

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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
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
    
}

void MainComponent::setMidiInput()
{
    auto list = juce::MidiInput::getAvailableDevices();

    auto newInput = list[0];

    if (! deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

    deviceManager.addMidiInputDeviceCallback (newInput.identifier, this);

}
