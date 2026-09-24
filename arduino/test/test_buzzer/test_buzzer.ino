#include <Arduino.h>

// Note frequencies for basic notes (C4, D4,...)
const float noteFreq[] = {
  // C3 - G7: Standard Major scale in Octaves:
    32.70f , 40.00f , 46.25f , 49.00f , 52.50f ,
    55.00f , 59.05f , 65.41f , 69.30f , 73.42f,
  // C4 - G7: Standard Major scale in Octaves:
    130.81f, 146.83f, 165.00f, 173.08f, 196.00f ,
    209.91f, 261.63f , 293.66f , 329.64f , 349.23f
};

// Note duration represented in milliseconds (1/4 = 250ms, 1/8 = 125ms)
unsigned long noteDurations[] = {
   256, 512, 793, 1063, 1300,
   1600,2132, 2796, 3520, 4186
};

void setup() {
    Serial.begin(9600);
}

void loop() {
  // play one octave C4 to C5
  for (int i = 0; i < 12; ++i) {
          
      tone(9, i % 7 * 3, noteDurations[i]);
      delay(noteDurations[i]);

      // stop the sound as soon as possible, to save space for other sounds
      noTone(9);
  }

  // wait a while between octaves (octave = C4-C5)
  int timeBetweenNotes = 10 * 1000; 
  int timeSoFar = 0;
  
  delay(timeBetweenNotes);

  Serial.println("finished playing");
}