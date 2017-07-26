#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "MPEHandler.h"
#include "SynthVoice.h"


MainContentComponent::MainContentComponent()
{

	windowWidth = 1000;
	windowHeight = 700;
	timeInterval = 30;
	started = false;
	startToggleClicksCount = 0;
	defaultButtonColour = Colours::cadetblue;
	MPEHandle = new MPEHandler(this);
	midiOutputDeviceIndex = -1;
	setSize(windowWidth, windowHeight);
	audioDevManager.initialise(0, 2, nullptr, true, String(), nullptr);
	audioDevManager.addMidiInputCallback(String(), this);
	//audioDevManager.addAudioCallback(this);
	StringArray devices = MidiInput::getDevices();
	deviceName = "";
	for (int i = 0;i < devices.size();i++) 
	{
		Logger::outputDebugString(devices[i]);
		if (devices[i].substring(0,4) == "ROLI")
		{
			Logger::outputDebugString(devices[i]);
			deviceName = devices[i];
		}
		else {
			midiOutputDeviceIndex = i;
			
			
		}
		
		if (deviceName != "" && midiOutputDeviceIndex > -1) {
			break;
		}
	}

	Logger::outputDebugString("out" + String(midiOutputDeviceIndex));
	midiOutputDevice = MidiOutput::openDevice(midiOutputDeviceIndex);
	audioDevManager.setMidiInputEnabled(deviceName, true);

	visInstrument.addListener(MPEHandle);
	

	/*for (int i = 0; i < 15; ++i) {
		synthesiser.addVoice(new SynthVoice);
	}

	synthesiser.enableLegacyMode(24);*/
	visInstrument.enableLegacyMode(24);
	//synthesiser.setVoiceStealingEnabled(false);

	initUIElements();
	
}

ScopedPointer<TextButton> MainContentComponent::addButton(String text, String name, Rectangle<int>  bounds, Colour colour)
{
	ScopedPointer<TextButton> button = new TextButton(String());
	button->setButtonText(text);
	button->setBounds(bounds);
	button->setName(name);
	button->setColour(TextButton::buttonColourId, colour);
	addAndMakeVisible(button);
	return button;
}

void MainContentComponent::initUIElements()
{

	int buttonWidth = 100;
	int buttonHeight = 30;
	int buttonSpacing = 10;
	int buttonPosY = 60;
	int buttonCount = 5;
	int buttonTotalArea = buttonCount*(buttonWidth + buttonSpacing);
	int buttonPosX = windowWidth/2 - buttonTotalArea/2;

	saveImageOption = addButton("Save Image","save_image",Rectangle<int>(buttonPosX, buttonPosY, buttonWidth, buttonHeight), defaultButtonColour);
	saveImageOption->addListener(this);

	buttonPosX += buttonWidth + buttonSpacing;
	saveTrackOption = addButton("Save Track", "save_track", Rectangle<int>(buttonPosX, buttonPosY, buttonWidth, buttonHeight), defaultButtonColour);
	saveTrackOption->addListener(this);

	buttonPosX += buttonWidth + buttonSpacing;
	loadTrackOption = addButton("Load Track", "load_track", Rectangle<int>(buttonPosX, buttonPosY, buttonWidth, buttonHeight), defaultButtonColour);
	loadTrackOption->addListener(this);

	buttonPosX += buttonWidth + buttonSpacing;
	startToggleOption = addButton("Start", "start", Rectangle<int>(buttonPosX, buttonPosY, buttonWidth, buttonHeight), defaultButtonColour);
	startToggleOption->addListener(this);

	buttonPosX += buttonWidth + buttonSpacing;
	clearGraphicsOption = addButton("Clear", "clear_graphics", Rectangle<int>(buttonPosX, buttonPosY, buttonWidth, buttonHeight), defaultButtonColour);
	clearGraphicsOption->addListener(this);

	

	const double visFullWidthPercent = 5;
	const double visFullHeightPercent = 20;
	const double viewPortHeightPercent = 0.8;
	const double viewPortWidthPercent = 1;

	visActualWidth = (visFullWidthPercent * windowWidth);
	visActualHeight = visFullHeightPercent * windowHeight;
	viewPortActualWidth = (viewPortWidthPercent * windowWidth);
	viewPortActualHeight = (viewPortHeightPercent * windowHeight);

	Rectangle<int> r(getLocalBounds());

	visualiserView.setBounds(0,windowHeight-viewPortActualHeight,viewPortActualWidth,viewPortActualHeight);
	MPEHandle->visualiser->setBounds(Rectangle<int>(visActualWidth,visActualHeight));
	visualiserView.setScrollBarsShown(false,false,true,true);
	visualiserView.setViewPositionProportionately(0.5, 0.0);
	addAndMakeVisible(visualiserView);

}

void  MainContentComponent::buttonClicked(Button* button)
{
	String buttonName = button->getName();
	if (buttonName == "save_image") 
	{
		File fileToSave = showFileBrowser("save", "*.png");
		MPEHandle->visualiser->saveGraphicsAsImage(fileToSave);
	}
	else if (buttonName == "save_track")
	{
		File fileToSave = showFileBrowser("save", "*.txt");
		MPEHandle->trackHandle->saveTrackAsText(fileToSave);
	}
	else if (buttonName == "start")
	{
		if (startToggleClicksCount % 2 == 0)
		{
			start();
			startToggleOption->setColour(TextButton::buttonColourId, Colours::orangered);
			startToggleOption->setButtonText("Stop");
		}
		else 
		{
			stop();
			startToggleOption->setColour(TextButton::buttonColourId, defaultButtonColour);
			startToggleOption->setButtonText("Start");
		}
	}
	else if (buttonName == "load_track") 
	{
		File fileToOpen = showFileBrowser("open", "*.txt");
		if (fileToOpen.exists())
		{
			MPEHandle->trackHandle->loadTrackFromText(fileToOpen, MPEHandle->visualiser);
			loadTrackOption->setButtonText("Loaded :)");
		}
	}
	else if (buttonName == "clear_graphics")
	{
		MPEHandle->visualiser->clearGraphics();
		MPEHandle->visualiser = new Visualiser(this);
	}
}
void MainContentComponent::paint(Graphics& g)
{
	g.setFont(Font(16.0f));
	g.setColour(Colours::white);
	String headerMsg = (deviceName == "") ? "No devices found" : "Your " + deviceName + " is connected!";
	g.drawText(headerMsg, 0, 0, windowWidth, 40, Justification::centred, true);
	
}


void  MainContentComponent::start()
{
	startToggleClicksCount++;
	MPEHandle->visualiser->startTimer(timeInterval);
	if (MPEHandle->trackHandle->isLoadingFile == true) {
		MPEHandle->trackHandle->startTimer(timeInterval);
		MPEHandle->trackHandle->isLoadingFile = false;
		MPEHandle->mainComponent->loadTrackOption->setColour(TextButton::buttonColourId, defaultButtonColour);
		loadTrackOption->setButtonText("Load Track");
	}
	started = true;
}
void  MainContentComponent::stop()
{
	startToggleClicksCount++;
	if (MPEHandle->visualiser->isTimerRunning()) { MPEHandle->visualiser->stopTimer(); }
	if (MPEHandle->trackHandle->isTimerRunning()) { MPEHandle->trackHandle->stopTimer(); }
	startToggleOption->setButtonText("Start");
	startToggleOption->setColour(TextButton::buttonColourId, defaultButtonColour);
	started = false;

}


/*
void MainContentComponent::audioDeviceIOCallback(const float** , int,
	float** outputChannelData, int numOutputChannels,
	int numSamples)
{
	// make buffer
	AudioBuffer<float> buffer(outputChannelData, numOutputChannels, numSamples);

	// clear it to silence
	buffer.clear();

	MidiBuffer incomingMidi;

	// get the MIDI messages for this audio block
	midiCollector.removeNextBlockOfMessages(incomingMidi, numSamples);

	// synthesise the block
	synthesiser.renderNextBlock(buffer, incomingMidi, 0, numSamples);
}


void MainContentComponent::audioDeviceAboutToStart(AudioIODevice* device)
{
	const double sampleRate = device->getCurrentSampleRate();
	midiCollector.reset(sampleRate);
	synthesiser.setCurrentPlaybackSampleRate(sampleRate);
}

void MainContentComponent::audioDeviceStopped()
{
}
*/
void MainContentComponent::output(String msg)
{
	Logger::outputDebugString(msg);
}

File MainContentComponent::showFileBrowser(String type, String fileTypeEnding)
{
	Logger::outputDebugString("msg");
	FileChooser choose("Choose a location to save...",
		File::getSpecialLocation(File::userHomeDirectory),
		fileTypeEnding);
	bool result = (type == "open") ? choose.browseForFileToOpen() : choose.browseForFileToSave(true);
	if (result==true)
	{
		File file(choose.getResult());
		return file;
	}
	else
	{
		return File();
	}

}

void MainContentComponent::handleIncomingMidiMessage(MidiInput*, const MidiMessage& message)
{
	if (started == true) {
		visInstrument.processNextMidiEvent(message);
		//midiCollector.addMessageToQueue(message);
	}
}

MidiMessage MainContentComponent::MPEToMidiMessage(MPENote note, int eventCalledFrom) {
	MidiMessage newMidiMessage;
	switch (eventCalledFrom)
	{
	case 1:
		newMidiMessage = MidiMessage::noteOn(note.midiChannel, note.initialNote, note.noteOnVelocity.asUnsignedFloat());
		break;
	case 2:
		newMidiMessage = MidiMessage::aftertouchChange(note.midiChannel, note.initialNote, note.pressure.as7BitInt());
		break;
	case 3:
		newMidiMessage = MidiMessage::pitchWheel(note.midiChannel, note.pitchbend.as14BitInt());
		break;
	case 4:
		newMidiMessage = MidiMessage::pitchWheel(note.midiChannel, note.pitchbend.as14BitInt());
		break;
	case 5:
		newMidiMessage = MidiMessage::noteOff(note.midiChannel, note.initialNote, note.noteOffVelocity.asUnsignedFloat());
		break;

	}

	return newMidiMessage;
}

