# MIDI_Viz

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
