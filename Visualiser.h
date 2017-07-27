#pragma once
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
class visNote : public Component 
{
	public:

		float notePressure;
		float noteTimbre;
		float notePitchBend;
		String visType;
		visNote(float pressure, float timbre, float pitchBend, String vType)
		{
			notePressure = pressure;
			noteTimbre = timbre;
			notePitchBend = pitchBend;
			visType = vType;
		}

		void paint(Graphics& g) override
		{
			int size = (notePressure * 100);
			size = (size < 5) ? 5 : size;
			int takeOff = noteTimbre * 255;

			if (visType == "Spiral") { size = size / 6; }
			
			uint8 red = uint8((notePressure*255));
			uint8 green = uint8((255 - noteTimbre*255));
			uint8 blue = uint8((notePitchBend*255));

			g.setColour(Colour(red,green,blue));
			int xInBound = (int) ((150 / 2) - (size / 2));
			g.drawEllipse(xInBound, 5, size, size, 10);
		}
};

class Visualiser : public Component,
				   public Timer
				   {
	public: 
		Image visImage;
		vector<MPENote> playedNotes;
		vector<int> noteTimes;
		int visBendSen = 1000;
		static const int noteBoundSize = 150;
		int xSen = 37;
		int timeSen = 1;
		int timeCount = 0;
		int noteFreq = 5;
		int yPos = timeSen;
		int xPos;
		int noteCounter = 0;
		int endImgX = 0;
		int startImgY = 0;
		int startImgX = 10000;
		int endImgY = 0;
		int scrollStep = 200;
		int scrollLeftX = 0;
		int latestNoteNum = 0;
		int currOctave = 1;
		int radius = 100;
		int lastOctave = 0;
		float spiralStartY;
		float spiralStartX;
		static const int spiralNoteSizeDivider = 6;
		String visType = "";
		bool firstScrollX = true;
		int spiralBearing;
		vector<int> octaves = {12,36,60,84,108,132};
		MainContentComponent *mainComponent;

		Visualiser(MainContentComponent *mainComp)
		{
			mainComponent = mainComp;
			visImage = Image(Image::RGB, 500, 500, true);
			mainComponent->visualiserView.setViewedComponent(this, false);
			setBounds(Rectangle<int>(mainComponent->visActualWidth, mainComponent->visActualHeight));
			lastOctave = currOctave;
			visType = mainComponent->visType;
		}
		void paint(Graphics& g) override
		{
			g.fillAll(Colours::white);
			g.setColour(Colour(255, 0, 0));

		}

		float startRadius = 10;
		float spiralSpacing = 2.2;

		void timerCallback() override
		{
			timeCount++;
			graphicsUpdaterOnTimer(visType);
			autoViewPosition();
			
		}

		void clearGraphics() 
		{
			mainComponent->stop();
			delete this;
		}

		void autoViewPosition()
		{
			if (visType == "Spiral") 
			{
				if (noteCounter == 0) {
					int startViewPosX = (mainComponent->visActualWidth / 2) - (mainComponent->viewPortActualWidth / 2);
					int startViewPosY = (mainComponent->visActualHeight / 2) - (mainComponent->viewPortActualHeight / 2);
					mainComponent->visualiserView.setViewPosition(startViewPosX, startViewPosY);
					startImgY = 10000;
				}
				else if(timeCount % 100 == 0){
					Logger::outputDebugString("tring to repos");
					int viewPosX = xPos - (mainComponent->viewPortActualWidth / 2);
					int viewPosY = yPos - (mainComponent->viewPortActualHeight / 2);
					mainComponent->visualiserView.setViewPosition(viewPosX, viewPosY);
				}
			}
			else if (visType == "Normal")
			{
				if (yPos % scrollStep == 0)
				{
					mainComponent->visualiserView.setViewPosition(mainComponent->visualiserView.getViewPosition().getX(), yPos - scrollStep);
				}

				if (currOctave != lastOctave || firstScrollX == true)
				{
					mainComponent->visualiserView.setViewPosition(currOctave*xSen, mainComponent->visualiserView.getViewPosition().getY());
					lastOctave = currOctave;
					firstScrollX = false;
				}
			}
		}

		void graphicsUpdaterOnDraw( MPENote note)
		{
			if (visType == "Spiral")
			{
				spiralStartX = mainComponent->visActualWidth / 2;
				spiralStartY = mainComponent->visActualHeight / 2;

				int octavePusher = currOctave * 5;
				float fullRadius = (startRadius + (timeCount / spiralSpacing)) + (note.initialNote*5) - octavePusher + note.pitchbend.asSignedFloat()*150;
				yPos = fullRadius * cos(spiralBearing * (M_PI / 180)) + spiralStartY;
				xPos = fullRadius * sin(spiralBearing * (M_PI / 180)) + spiralStartX;

				startImgY = (startImgY > yPos) ? yPos : startImgY;
				endImgY = (endImgY < yPos) ? yPos : endImgY;
			}
			else if (visType == "Normal")
			{
				xPos = (note.initialNote * xSen) + note.pitchbend.asSignedFloat() * visBendSen;
				
				if (noteCounter == 0){startImgY = yPos;}else{endImgY = yPos;}
			}
			endImgX = (endImgX < xPos) ? xPos : endImgX;
			startImgX = (startImgX > xPos) ? xPos : startImgX;
		}

		void graphicsUpdaterOnTimer(String type)
		{
			if (visType == "Spiral")
			{
				spiralBearing = timeCount / 1;
			}
			else if (visType == "Normal")
			{
				yPos = yPos + timeSen;
			}
		}

		void saveGraphicsAsImage(File fileToSave)
		{
			int endY;
			if (visType == "Normal") {
				endY = endImgY + noteBoundSize;
			}
			else if (visType == "Spiral") {
				endY = (endImgY - startImgY) + noteBoundSize / spiralNoteSizeDivider;
			}
			int endX = (endImgX - startImgX) + noteBoundSize;
			Rectangle<int> graphicsArea = Rectangle<int>(startImgX,startImgY,endX, endY);
			visImage = this->createComponentSnapshot(graphicsArea, true, 5.0f);
			FileOutputStream streamFile = fileToSave;
			PNGImageFormat pngImage;
			pngImage.writeImageToStream(visImage, streamFile);
		}


		void drawNote(MPENote note)
		{
			if ((noteCounter % noteFreq ==0 ) && ( note.keyState == MPENote::keyDown || note.keyState == MPENote::keyDownAndSustained))
			{

				if ((visType == "Spiral" && noteCounter == 0) || visType == "Normal") {

					for (int i = 0;i < octaves.size() - 1; i++)
					{
						if (note.initialNote >= octaves[i] && note.initialNote < octaves[i + 1])
						{
							currOctave = octaves[i];
							break;
						}
					}
				}

				playedNotes.push_back(note);
				noteTimes.push_back(timeCount);

				float velocity = note.noteOnVelocity.asUnsignedFloat();
				float pressure = note.pressure.asUnsignedFloat();
				float timbre = note.timbre.asUnsignedFloat();
				float pitchBend = note.pitchbend.asSignedFloat();
				float pitchBendUn = note.pitchbend.asUnsignedFloat();

				visNote* newVisNote = new visNote(pressure,timbre,pitchBendUn, visType);
				graphicsUpdaterOnDraw(note);
				newVisNote->setBounds(xPos, yPos, noteBoundSize, noteBoundSize);
				addAndMakeVisible(newVisNote);
			}
			noteCounter++;
		}
};