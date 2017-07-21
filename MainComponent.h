#pragma once
class MPEHandler;
using namespace std;
class MainContentComponent : public Component,
							// private AudioIODeviceCallback,
							 private MidiInputCallback,
							 public Button::Listener
 {
	public:
		String deviceName;
		int windowWidth;
		int windowHeight;
		int timeInterval;
		bool started;
		Colour defaultButtonColour;
		int startToggleClicksCount;
		OwnedArray<Label> MPELabels;
		AudioDeviceManager audioDevManager;
		MPEInstrument visInstrument;
		//MPESynthesiser synthesiser;
		//MidiMessageCollector midiCollector;
		ScopedPointer<MPEHandler> MPEHandle;
		ScopedPointer<TextButton> saveImageOption;
		ScopedPointer<TextButton> saveTrackOption;
		ScopedPointer<TextButton> loadTrackOption;
		ScopedPointer<TextButton> startToggleOption;
		ScopedPointer<TextButton> clearGraphicsOption;

		double visActualWidth;
		double visActualHeight;

		Viewport visualiserView;
		MainContentComponent();

		void initUIElements();
 
		void paint(Graphics& g);

		void output(String msg);

	/*	void audioDeviceIOCallback(const float**, int,
			float** outputChannelData, int numOutputChannels,
			int numSamples) override;

		void audioDeviceAboutToStart(AudioIODevice* device) override;

		void audioDeviceStopped();*/

		void stop();
		void start();

		ScopedPointer<TextButton> addButton(String text, String name, Rectangle<int>  bounds, Colour colour);

		void buttonClicked(Button* button) override;

		File MainContentComponent::showFileBrowser(String type, String fileTypeEnding);

	private:

		void handleIncomingMidiMessage(MidiInput*, const MidiMessage& message) override;
	/*	void audioDeviceIOCallback(const float**, int,
			float** outputChannelData, int numOutputChannels,
			int numSamples) override;*/


		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
