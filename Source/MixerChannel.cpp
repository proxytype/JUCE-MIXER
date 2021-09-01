/*
  ==============================================================================

    MixerChannel.cpp
    Created: 30 Aug 2021 2:22:59am
    Author:  proxytype

  ==============================================================================
*/

#include "MixerChannel.h"

MixerChannel::MixerChannel() {

}

MixerChannel::MixerChannel(juce::String channel)
{
    channelName = channel;

    setAudioChannels(0, 2);
    thread.startThread();
    formatManager.registerBasicFormats();
    audioDeviceManager.addAudioCallback(&audioSourcePlayer);
    audioSourcePlayer.setSource(&transportSource);

    addAndMakeVisible(sliderChannel);
    addAndMakeVisible(panningChannel);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(startStopButton);
    addAndMakeVisible(lbl);

    setSize(130, 500);
}

void MixerChannel::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MixerChannel::resized()
{

    lbl.setBounds(55, 0, 100, 20);
    lbl.setFont(juce::Font(24.0f, juce::Font::bold));
    lbl.setText(channelName, juce::dontSendNotification);

    loadButton.setBounds(30, 30, 90, 20);
    loadButton.onClick = [this] {openFile(0); };

    startStopButton.setBounds(10, 30, 20, 20);
    startStopButton.onClick = [this] {startOrStop(); };

    panningChannel.setBounds(35, 50, 70, 70);
    panningChannel.setRange(-1.0, 1.0);
    panningChannel.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    panningChannel.setSliderStyle(juce::Slider::SliderStyle::Rotary);

    sliderChannel.setBounds(10, 110, 120, 200);
    sliderChannel.setRange(0.0, 1.0);
    sliderChannel.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    sliderChannel.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);

  
}

void MixerChannel::openFile(int channel)
{
    if (fileChooser != nullptr)
        return;

    if (!juce::RuntimePermissions::isGranted(juce::RuntimePermissions::readExternalStorage)) {
        juce::Component::SafePointer<MixerChannel> safeThis(this);
        juce::RuntimePermissions::request(juce::RuntimePermissions::readExternalStorage, [safeThis, channel](bool granted) mutable {
            if (safeThis != nullptr && granted) {
                safeThis->openFile(channel);
            }
        });

        return;
    }

    fileChooser.reset(new juce::FileChooser("Select an audio file...", juce::File(), "*.wav;*.mp3;*.aif"));

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this, channel](const juce::FileChooser& fc) mutable {

        if (fc.getURLResults().size() > 0) {
            auto u = fc.getURLResult();

            bool status = false;
            if (channel == 0) {
                loadURL(u, channel);
            }
            else {
                //unable to load file
            }

        }

        fileChooser = nullptr;
    }, nullptr);
}

bool MixerChannel::startOrStop()
{
    if (transportSource.isPlaying())
    {
        transportSource.stop();
    }
    else
    {
        transportSource.setPosition(0);
        transportSource.start();

    }

    return true;
}

void MixerChannel::releaseResources()
{
}

MixerChannel::~MixerChannel()
{
    transportSource.setSource(nullptr);
    audioSourcePlayer.setSource(nullptr);
    audioDeviceManager.removeAudioCallback(&audioSourcePlayer);
    this->shutdownAudio(); 
}

bool MixerChannel::loadURL(juce::URL& fileToPlay, int channel)
{
    juce::AudioFormatReader* reader = nullptr;

    transportSource.stop();
    transportSource.setSource(nullptr);
    currentAudioFileSource.reset();

    if (fileToPlay.isLocalFile())
    {
        loadButton.setTitle(fileToPlay.getFileName());
        reader = formatManager.createReaderFor(fileToPlay.getLocalFile());
    }

    if (reader != nullptr)
    {

        currentAudioFileSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(currentAudioFileSource.get(),
            32768,                   // tells it to buffer this many samples ahead
            &thread,                 // this is the background thread to use for reading-ahead
            reader->sampleRate);     // allows for sample rate correction

        return true;
    }

    return false;
}

void MixerChannel::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MixerChannel::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (currentAudioFileSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);

    auto level = (float)sliderChannel.getValue();
    auto pan = (float)panningChannel.getValue();

    float rightLevel = level;
    float leftLevel = level;

    if (bufferToFill.buffer->getNumChannels() > 1) {

        auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        if (pan > 0) {
            leftLevel = leftLevel * (1 - abs(pan));
        }
        else if (pan < 0) {
            rightLevel = rightLevel * (1 - abs(pan));
        }

        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            leftChannel[sample] = leftChannel[sample] * leftLevel;
            rightChannel[sample] = rightChannel[sample] * rightLevel;
        }
    }
}
