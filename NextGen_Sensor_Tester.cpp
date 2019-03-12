#include <Arduino.h>
#include <FreqCount.h>
// useful video from the originator of the eclipse integration:
// https://www.youtube.com/watch?v=TtPvkPpAx0E
// Needs the FreqCount library (right click on the Project -> Properties -> Libraries)

#define DEBUG
enum colours {
	GREEN = 0, RED = 1
};

// function prototypes
void LED_RED();
void LED_GREEN();
void LED_OFF();

void LED_BLINK(colours);

// globals
byte blink_counter = 0;

void setup() {
	pinMode(8, OUTPUT);
	pinMode(10, OUTPUT);
	digitalWrite(8, LOW);
	digitalWrite(10, LOW);

#ifdef DEBUG
	Serial.begin (115200);
#endif

	FreqCount.begin(250);	// measure for 1/4 seconds
	blink_counter = 0;

}

void loop() {

	static unsigned long report = 0;
	// put your main code here, to run repeatedly:

	if (!FreqCount.available())
		return;

	unsigned long count = FreqCount.read();
#ifdef DEBUG
	Serial.print (F("\nReading number "));
	Serial.print (report++);
	Serial.print (F(",  reading = "));
	Serial.println (count);
#endif

	// Now interpret the results
	// Case 1: no reading.  Disconnected
	if (count < 50) // should test for 0, but sometimes get a few spurious pulses through
			{
#ifdef DEBUG
		Serial.println (F("disconnected"));
#endif
		LED_BLINK(RED);
		return;
	}

	// Case 2: count > 20,000: sensor is in air
	// 2018-06-08: Cougar: > 20000 for air
	if (count > 20000) {  // 35000 for bare board, 20000 for assembled sensor
#ifdef DEBUG
		Serial.println (F("air reading"));
#endif
		LED_BLINK(GREEN);
		return;
	}

	blink_counter = 0; // just makes sure that the flashing starts as soon as we go back to flashing mode

	// if the water is about half way up, the reading is about 2000.
	// So decide if we're above of below this.
	// 2018-06-08: Cougar: 2500
	if (count > 2500) {
#ifdef DEBUG
		Serial.println (F("wet reading"));
#endif
		LED_GREEN();
		return;
	} else {
#ifdef DEBUG
		Serial.println (F("alarm reading"));
#endif
		LED_RED();
		return;
	}

	//    if (count < 6000) {
	//      float f = (6000 - count) / 294.0;
	//      Serial.print ("  ");
	//      Serial.print (f);
	//      Serial.println (F(" mm"));
	//    }
	//} // if FreqCount.available()

}

void LED_RED() {
	digitalWrite(8, HIGH);
	digitalWrite(10, LOW);
}


void LED_GREEN() {
	digitalWrite(8, LOW);
	digitalWrite(10, HIGH);
}

void LED_OFF() {
	digitalWrite(8, LOW);
	digitalWrite(10, LOW);
}

void LED_BLINK(colours colour) {
	if (blink_counter == 0) {
		if (colour == RED)
			LED_RED();
		else
			LED_GREEN();

		delay(50);
		LED_OFF();
	}
	if (++blink_counter == 2)
		blink_counter = 0;
}

