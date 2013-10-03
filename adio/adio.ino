// constants
static const int NumberOfArpeggioPotentiometers = 5;
static const int ToneOutPin = 11;
// values
uint16_t arpeggioPotentiometers[NumberOfArpeggioPotentiometers] = { 0 };
uint8_t currentBeat = 0;
int8_t direction = 1; // 1 or -1
bool needUpdate = false;

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
// implementations
void setup()
{
	pinMode(ToneOutPin, OUTPUT);

	for (int i = 0; i < NumberOfArpeggioPotentiometers; ++i)
	{
		arpeggioPotentiometers[i] = 0;
	}

  // Timer2 PWM Mode set to fast PWM 
  cbi (TCCR2A, COM2A0);
  sbi (TCCR2A, COM2A1);
  sbi (TCCR2A, WGM20);
  sbi (TCCR2A, WGM21);

  cbi (TCCR2B, WGM22);

  // Timer2 Clock Prescaler to : 1 
  sbi (TCCR2B, CS20);
  cbi (TCCR2B, CS21);
  cbi (TCCR2B, CS22);

  // Timer2 PWM Port Enable
  sbi(DDRB,3);                    // set digital pin 11 to output

  //cli();                         // disable interrupts to avoid distortion
  cbi (TIMSK0,TOIE0);              // disable Timer0 !!! delay is off now
  Serial.begin(9600);
  Serial.println("hoge");
}

void loop()
{
       const uint8_t arpeggioValue = analogRead(currentBeat) >> 2;
       uint32_t wait = analogRead(5);
       wait *= 1000;
       OCR2A = arpeggioValue;
       Serial.print(currentBeat);
       Serial.print(", ");
       Serial.println(arpeggioValue);
       
       for (uint32_t i = 0; i < wait; ++i) {
         __asm__("nop\n\t"); 
       }
       newBeat();
}

void newBeat()
{
	currentBeat += direction;

	if (currentBeat >= NumberOfArpeggioPotentiometers || currentBeat < 0)
	{

		direction *= -1;

		currentBeat += direction;
		currentBeat += direction;
	}

	needUpdate = true;
}
