#include "Visualiser.h"
#include "TrackHandler.h"

class MPEHandler : public Component,
				   public MPEInstrument::Listener{
	public:

		bool started = false;
		int timeInterval = 10;
		Visualiser *visualiser = new Visualiser();
		TrackHandler *trackHandle = new TrackHandler();
		MPEHandler() {

		}
		MainContentComponent *mainComponent;

		void getMC(MainContentComponent *mainContentComponent){
			mainComponent = mainContentComponent;
		}

		void start() {
			visualiser->startTimer(timeInterval);
			if (trackHandle->isLoadingFile == true) {
				trackHandle->startTimer(timeInterval);
				trackHandle->isLoadingFile = false;
				mainComponent->loadTrackOption->setColour(TextButton::buttonColourId,mainComponent->defaultButtonColour);
				mainComponent->loadTrackOption->setButtonText("Load Track");
			}
			started = true;
		}
		void stop() {
			if (visualiser->isTimerRunning()) { visualiser->stopTimer(); }
			if (trackHandle->isTimerRunning()) { trackHandle->stopTimer(); }
			mainComponent->startToggleOption->setButtonText("Start");
			mainComponent->startToggleOption->setColour(TextButton::buttonColourId, mainComponent->defaultButtonColour);
			started = false;
		}

		void recievedMidiMiddleProcess(MPENote note){
			struct DrawCallback : public CallbackMessage
			{
				DrawCallback(MPEHandler& o, const MPENote& n,Visualiser* v) : owner(o), note(n), vis(v) {}

				void messageCallback() override
				{

					owner.visualiser->drawNote(note);
					owner.trackHandle->playedNotes.push_back(note);
					owner.trackHandle->playedNoteTimes.push_back(vis->timeCount);
				}
				MPEHandler& owner;
				Visualiser* vis;
				MPENote note;
			};

			(new DrawCallback(*this, note, visualiser))->post();
		}
		void noteAdded(MPENote note) override {
			recievedMidiMiddleProcess(note);
		}
		void notePressureChanged(MPENote note) override {
			recievedMidiMiddleProcess(note);
		}
		void notePitchbendChanged(MPENote note) override {
			recievedMidiMiddleProcess(note);
		}
		void noteTimbreChanged(MPENote note) override {
			recievedMidiMiddleProcess(note);
		}
		void noteKeyStateChanged(MPENote note) override {
			recievedMidiMiddleProcess(note);
		}
		void noteReleased(MPENote note) override {
			recievedMidiMiddleProcess(note);
		}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MPEHandler)
};
