#include "MainComponent.h"

MainComponent::MainComponent()
{
    

    addAndMakeVisible(a1);
    addAndMakeVisible(a2);
    addAndMakeVisible(masterSlider);
    addAndMakeVisible(lbl);

    setSize(390, 340);
}

MainComponent::~MainComponent()
{
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
}

void MainComponent::releaseResources()
{
}

void MainComponent::masterValueChange() {
    juce::SystemAudioVolume::setGain(masterSlider.getValue());
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    /*  g.setFont (juce::Font (16.0f));
      g.setColour (juce::Colours::white);
      g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);*/
}

void MainComponent::resized()
{
    a1.setBounds(0, 20, 140, 450);
    a2.setBounds(140, 20, 140, 450);

    lbl.setBounds(308, 160, 100, 20);
    lbl.setFont(juce::Font(14.0f, juce::Font::bold));
    lbl.setText("MASTER", juce::dontSendNotification);

    masterSlider.setBounds(320, 180, 40, 150);
    masterSlider.setRange(0.0, 1.0);
    masterSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    masterSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    masterSlider.onValueChange = [this] {masterValueChange(); };
   
}
