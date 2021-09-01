/*
  ==============================================================================

    MixerChannel.h
    Created: 30 Aug 2021 2:22:38am
    Author:  Pandora

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class MixerChannel : public juce::AudioAppComponent
{
public:

    juce::TimeSliceThread thread{ "audio file preview" };
    juce::Slider sliderChannel;
    juce::Slider panningChannel;
    juce::TextButton loadButton{ "Load File..." };
    juce::TextButton startStopButton{ ">" };
    juce::AudioFormatManager  formatManager;
    juce::AudioDeviceManager audioDeviceManager;
    juce::Label lbl;
    

    MixerChannel();
    MixerChannel(juce::String channel);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void openFile(int channel);
    bool startOrStop();
    void releaseResources() override;

    ~MixerChannel() override;

private:
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> currentAudioFileSource;
    std::unique_ptr<juce::AudioFormatReaderSource> audioFileSource;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::String channelName{ "" };
    bool loadURL(juce::URL& fileToPlay, int channel);

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
};
