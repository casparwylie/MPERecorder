#pragma once
class MPEHandler;
using namespace std;
class MainContentComponent : public Component,
							 private MidiInputCallback,
							 public Button::Listener,
							 public ComboBox::Listener
 {
	public:
		String deviceName;
		int windowWidth;
		int windowHeight;
		int timeInterval;
		int midiOutputDeviceIndex;
		bool started;
		int deviceInID;
		String visType;
		Colour defaultButtonColour;
		int startToggleClicksCount;
		OwnedArray<Label> MPELabels;
		AudioDeviceManager audioDevManager;
		MPEInstrument visInstrument;
		ScopedPointer<MPEHandler> MPEHandle;
		ScopedPointer<MidiOutput> midiOutputDevice;
		ScopedPointer<TextButton> saveImageOption;
		ScopedPointer<TextButton> saveTrackOption;
		ScopedPointer<TextButton> loadTrackOption;
		ScopedPointer<TextButton> startToggleOption;
		ScopedPointer<TextButton> clearGraphicsOption;
		ScopedPointer<ComboBox> chooseVisTypeBox;
		ScopedPointer<ComboBox> chooseMidiInputBox;
		ScopedPointer<ComboBox> chooseMidiOutputBox;
		ScopedPointer<Label> chooseMidiOutputLabel;
		ScopedPointer<Label> chooseMidiInputLabel;

		double visActualWidth;
		double visActualHeight;
		double viewPortActualWidth;
		double viewPortActualHeight;

		Viewport visualiserView;
		MainContentComponent();

		void initUIElements();
 
		void paint(Graphics& g) override;

		void output(String msg);

		void stop();
		void start();

		ScopedPointer<TextButton> addButton(String text, String name, Rectangle<int>  bounds, Colour colour);

		ScopedPointer<ComboBox> addComboBox(StringArray optionList, String name, Rectangle<int>  bounds, Colour colour);

		void buttonClicked(Button* button) override;

		void comboBoxChanged(ComboBox* comboBox) override;

		File showFileBrowser(String type, String fileTypeEnding);

		MidiMessage MPEToMidiMessage(MPENote note, int eventCalledFrom);

	private:

		void handleIncomingMidiMessage(MidiInput*, const MidiMessage& message) override;


		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
