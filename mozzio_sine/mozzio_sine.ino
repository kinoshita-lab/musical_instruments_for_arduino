#include "pitches.h"
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <ADSR.h>

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
ADSR <CONTROL_RATE> envelope;
// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // powers of 2 please
// constants
static const int BeatInputPin = 0;
static const int ToneOutputPin = 6;
static const int AnalogValuePin = 5;
static const int NumberOfArpeggioPotentiometers = 5;

static const uint8_t NumberOfNotes = 37;
static const uint16_t NoteNumbers[NumberOfNotes] =
{ 0, 
  NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2, 
  NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3, 
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4
};

// values
uint16_t arpeggioPotentiometers[NumberOfArpeggioPotentiometers] = { 0 };
uint8_t currentBeat = 0;
int8_t direction = 1; // 1 or -1
bool needUpdate = false;

// function prototypes
void newBeat();

// implementations
void setup()
{
	pinMode(BeatInputPin, INPUT);
	pinMode(ToneOutputPin, OUTPUT);
	attachInterrupt(BeatInputPin, newBeat, RISING);

	for (int i = 0; i < NumberOfArpeggioPotentiometers; ++i)
	{
		arpeggioPotentiometers[i] = 0;
	}

        startMozzi(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
        envelope.setADLevels(255,255);
        aSin.setFreq(440);
}

void loop()
{
	if (needUpdate)
	{
		const uint16_t adValue = analogRead(currentBeat);
		const uint8_t noteIndex = map(adValue, 0, 1023, 0, NumberOfNotes);
		uint16_t duration = analogRead(AnalogValuePin) >> 1;
                duration = duration == 0 ? 1 : duration;
		needUpdate = false;
                aSin.setFreq((int)NoteNumbers[noteIndex]);
                envelope.setTimes(0u,duration,duration,0);  // "u" means unsigned int
                envelope.noteOn();
		tone(ToneOutputPin, NoteNumbers[noteIndex], duration);
	}

        audioHook(); // required here
}

void newBeat()
{
	noTone(ToneOutputPin);

	currentBeat += direction;

	if (currentBeat >= NumberOfArpeggioPotentiometers || currentBeat < 0)
	{

		direction *= -1;

		currentBeat += direction;
		currentBeat += direction;
	}

	needUpdate = true;
}

void updateControl(){
  envelope.update();
}


int updateAudio(){
   return (int) (envelope.next() * aSin.next())>>8; //return aSin.next(); // return an int signal centred around 0
}

