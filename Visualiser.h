#pragma once
class visNote : public Component 
{
	public:

		float notePressure;
		float noteTimbre;
		float notePitchBend;
		visNote(float pressure, float timbre, float pitchBend)
		{
			notePressure = pressure;
			noteTimbre = timbre;
			notePitchBend = pitchBend;
		}

		void paint(Graphics& g) override
		{
			int size = (notePressure * 100);
			size = (size < 5) ? 5 : size;
			uint8 red = uint8(notePressure*255);
			uint8 green = uint8(255 - noteTimbre*255);
			uint8 blue = uint8(notePitchBend*255);
			g.setColour(Colour(red,green,blue));
			g.drawEllipse(5, 5, size, size, (noteTimbre*10)+2);
		}
};

class Visualiser : public Component,
				   public Timer
				   {
	public: 
		Image visImage;
		vector<MPENote> playedNotes;
		vector<int> noteTimes;
		int visBendSen = 500;
		int noteBoundSize = 150;
		int XSen = 10;
		int timeSen = 1;
		int timeCount = 0;
		int noteFreq = 5;
		int yPos = timeSen;
		int noteCounter = 0;
		int lowestX = 10000;
		int startY = 0;
		int heighestX = 0;
		int scrollStep = 513;
		int lastDrawnY = 0;
		MainContentComponent *mainComponent;

		Visualiser(MainContentComponent *mainComp)
		{
			mainComponent = mainComp;
			visImage = Image(Image::RGB, 500, 500, true);
			mainComponent->visualiserView.setViewedComponent(this, false);
			setBounds(Rectangle<int>(mainComponent->visActualWidth, mainComponent->visActualHeight));
		}
		void paint(Graphics& g) override
		{
			g.fillAll(Colours::white);
		}

		void timerCallback() override
		{
			timeCount++;
			yPos = yPos + timeSen;
			int viewPosY = yPos+100;
			if (yPos % scrollStep == 0) 
			{
				mainComponent->visualiserView.autoScroll(0, yPos+scrollStep, 1000, scrollStep);
			}
		}

		void clearGraphics() 
		{
			mainComponent->stop();
			delete this;
		}

		void saveGraphicsAsImage(File fileToSave)
		{
			int endY = lastDrawnY + noteBoundSize;
			int endX = (heighestX - lowestX) + noteBoundSize;
			Rectangle<int> graphicsArea = Rectangle<int>(lowestX, startY, endX, endY);
			visImage = this->createComponentSnapshot(graphicsArea, true, 5.0f);
			FileOutputStream streamFile = fileToSave;
			PNGImageFormat pngImage;
			pngImage.writeImageToStream(visImage, streamFile);
		}


		void drawNote(MPENote note)
		{
			if ((noteCounter % noteFreq ==0 ) && ( note.keyState == MPENote::keyDown || note.keyState == MPENote::keyDownAndSustained))
			{
				playedNotes.push_back(note);
				noteTimes.push_back(timeCount);
				float velocity = note.noteOnVelocity.asUnsignedFloat();
				float pressure = note.pressure.asUnsignedFloat();
				float timbre = note.timbre.asUnsignedFloat();
				float pitchBend = note.pitchbend.asSignedFloat();
				float pitchBendUn = note.pitchbend.asUnsignedFloat();
				visNote* newVisNote = new visNote(pressure,timbre,pitchBendUn);
				int xPos = ((note.initialNote) * XSen) + pitchBend*visBendSen;
				lowestX = (lowestX > xPos) ? xPos : lowestX;
				heighestX = (heighestX < xPos) ? xPos : heighestX;
				newVisNote->setBounds(xPos, yPos, noteBoundSize, noteBoundSize);
				addAndMakeVisible(newVisNote);
				if (noteCounter == 0)
				{
					startY = yPos;
				}else
				{
					lastDrawnY = yPos;
				}
			}
			noteCounter++;
		}
};