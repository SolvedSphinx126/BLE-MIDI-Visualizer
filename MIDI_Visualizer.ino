#include <Arduino.h>
#include <BLEMidi.h>

int latchPin = 2;  // Latch pin of 74HC595 is connected to Digital pin 5
int clockPin = 15; // Clock pin of 74HC595 is connected to Digital pin 6

int dataPin1 = 4;  // Data pin of 74HC595 is connected to Digital pin 4
int dataPin2 = 16;
int dataPin3 = 17;
int dataPin4 = 5;
int dataPin5 = 18;
int dataPin6 = 19;
int dataPin7 = 21;
int dataPin8 = 3;
int dataPin9 = 1;
int dataPin10 = 22;
int dataPin11 = 23;

unsigned int idle = 0;
unsigned int idx = 0;
bool conn = false;
bool left = false;

int dataPins[11] = {dataPin1, dataPin2, dataPin3, dataPin4, dataPin5, dataPin6, dataPin7, dataPin8, dataPin9, dataPin10, dataPin11};
byte keys[11] = {};

void connected();

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  //Serial.printf("Received note on : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  // Sometimse a note on @ 0 velocity(volume) is used to represent a note off message
  if(velocity > 0)
    setBit(note - 21);
  else
    clearBit(note - 21);
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  //Serial.printf("Received note off : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  clearBit(note - 21);
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
    //Serial.printf("Received control change : channel %d, controller %d, value %d (timestamp %dms)\n", channel, controller, value, timestamp);
}

void connected()
{
  conn = true;
  //Serial.println("Connected");
}

void setup() {
  //Serial.begin(115200);
  BLEMidiServer.begin("MIDI device");
  BLEMidiServer.setOnConnectCallback(connected);
  BLEMidiServer.setOnDisconnectCallback([](){     // To show how to make a callback with a lambda function
    Serial.println("Disconnected");
    conn = false;
    idx = 0;
  });
  BLEMidiServer.setNoteOnCallback(onNoteOn);
  BLEMidiServer.setNoteOffCallback(onNoteOff);
  BLEMidiServer.setControlChangeCallback(onControlChange);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(dataPin1, OUTPUT);
  pinMode(dataPin2, OUTPUT);
  pinMode(dataPin3, OUTPUT);
  pinMode(dataPin4, OUTPUT);
  pinMode(dataPin5, OUTPUT);
  pinMode(dataPin6, OUTPUT);
  pinMode(dataPin7, OUTPUT);
  pinMode(dataPin8, OUTPUT);
  pinMode(dataPin9, OUTPUT);
  pinMode(dataPin10, OUTPUT);
  pinMode(dataPin11, OUTPUT);

  //BLEMidiServer.enableDebugging();
}

void loop() {
  // idle animation
  if(!conn & idx % 100 == 0) {
    if(!left and idle < 88) {
      clearBit(idle++);
      setBit(idle);
    } else if(!left and idle == 88) {
      left = true;
      clearBit(idle--);
      setBit(idle);
    } else if(left and idle > 0) {
      clearBit(idle--);
      setBit(idle);
    } else {
      left = false;
      clearBit(idle++);
      setBit(idle);
    }
  }
  idx = (idx + 1) % 100;
  updateShiftRegister();
}

void setBit(int bit)
{
  keys[bit / 8] |= (1 << 7 - (bit % 8));
}

void clearBit(int bit)
{
  keys[bit / 8] &= ~(1 << 7 - (bit % 8));
}

void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 11; j++)
    {
      digitalWrite(dataPins[j], keys[j] & (1 << i));
    }
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }
  digitalWrite(latchPin, HIGH);
}
