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
		int midiOutputDeviceIndex;
		bool started;
		Colour defaultButtonColour;
		int startToggleClicksCount;
		OwnedArray<Label> MPELabels;
		AudioDeviceManager audioDevManager;
		MPEInstrument visInstrument;
		//MPESynthesiser synthesiser;
		//MidiMessageCollector midiCollector;
		ScopedPointer<MPEHandler> MPEHandle;
		ScopedPointer<MidiOutput> midiOutputDevice;
		ScopedPointer<TextButton> saveImageOption;
		ScopedPointer<TextButton> saveTrackOption;
		ScopedPointer<TextButton> loadTrackOption;
		ScopedPointer<TextButton> startToggleOption;
		ScopedPointer<TextButton> clearGraphicsOption;

		double visActualWidth;
		double visActualHeight;
		double viewPortActualWidth;
		double viewPortActualHeight;

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

		File showFileBrowser(String type, String fileTypeEnding);

		MidiMessage MPEToMidiMessage(MPENote note, int eventCalledFrom);

	private:

		void handleIncomingMidiMessage(MidiInput*, const MidiMessage& message) override;
	/*	void audioDeviceIOCallback(const float**, int,
			float** outputChannelData, int numOutputChannels,
			int numSamples) override;*/


		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
