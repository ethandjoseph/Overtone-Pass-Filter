#pragma once
#include <JuceHeader.h>

juce::String getMidiNoteName(int midiNoteNumber)
{
    static const juce::StringArray noteNames = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (midiNoteNumber / 12) - 1;
    int noteIndex = midiNoteNumber % 12;
    return noteNames[noteIndex] + juce::String(octave);
}