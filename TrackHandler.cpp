#pragma once
#include "TrackHandler.h"
TrackHandler::TrackHandler() {

	int timeCount = 0;
	int noteCount = 1;
	isLoadingFile = false;

}

void TrackHandler::saveTrackAsText(File fileToSave) {
	File textFile = fileToSave;
	for (int i = 0;i < playedNotes.size();i++) {
		MPENote note = playedNotes[i];
		String dataRow = "";
		for (int x = 0;x <= 5;x++) {
			String MPEDisplayVal;

			switch (x) {
			case 0:
				MPEDisplayVal = String(note.midiChannel);
				break;
			case 1:
				MPEDisplayVal = String(note.initialNote);
				break;
			case 2:
				MPEDisplayVal = String(note.noteOnVelocity.as14BitInt());
				break;
			case 3:
				MPEDisplayVal = String(note.pitchbend.as14BitInt());
				break;
			case 4:
				MPEDisplayVal = String(note.pressure.as14BitInt());
				break;
			case 5:
				MPEDisplayVal = String(note.timbre.as14BitInt());
				break;
			default:
				MPEDisplayVal = "n/a";
				break;
			}
			dataRow = dataRow + MPEDisplayVal + ",";
		}

		dataRow = dataRow + String(playedNoteTimes[i]) + ",";
		textFile.appendText(dataRow);
	}
}

void TrackHandler::loadTrackFromText(File FileToOpen, Visualiser *vis, MPEHandler *mpeHand) {
	visualiser = vis;
	MPEHandle = mpeHand;
	playedNotes.clear();
	playedNoteTimes.clear();
	String trackDataStr = FileToOpen.loadFileAsString();
	isLoadingFile = true;
	int localCount = 0;
	int tempMidiChannel = 0;
	int tempInitialNote = 0;
	MPEValue tempVelocity;
	MPEValue tempPitchBend;
	MPEValue tempPressure;
	MPEValue tempTimbre;
	trackData.addTokens(StringRef(trackDataStr), StringRef(","), "");
	for (int i = 0;i < trackData.size();i++) {
		float dataAsFloat = trackData[i].getFloatValue();
		int dataAsInt = trackData[i].getIntValue();

		switch (localCount) {
		case 0:
			tempMidiChannel = dataAsInt;
			break;
		case 1:
			tempInitialNote = dataAsInt;
			break;
		case 2:
			tempVelocity = MPEValue::from14BitInt(dataAsInt);
			break;
		case 3:
			tempPitchBend = MPEValue::from14BitInt(dataAsInt);
			break;
		case 4:
			tempPressure = MPEValue::from14BitInt(dataAsInt);
			break;
		case 5:
			tempTimbre = MPEValue::from14BitInt(dataAsInt);
			break;
		case 6:
			localCount = -1;
			playedNoteTimes.push_back(trackData[i].getIntValue());
			MPENote newNote = MPENote(tempMidiChannel, tempInitialNote, tempVelocity, tempPitchBend, tempPressure, tempTimbre);
			playedNotes.push_back(newNote);
			break;

		}
		localCount++;
	}
}

void TrackHandler::timerCallback() {
	for (int i = 0;i < playedNoteTimes.size();i++) {
		if (playedNoteTimes[i] == timeCount) {
			visualiser->drawNote(playedNotes[i]);
		}
		if (i == playedNoteTimes.size() - 1) {
			MPEHandle->stop();
		}
	}
	timeCount++;
}
