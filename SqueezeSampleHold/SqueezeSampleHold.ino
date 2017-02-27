#include <DAC8554.h>

const uint8_t channelHigh = 0;
const uint8_t channelLow = 1;

const uint8_t LPCIn = A5;
const uint8_t DACSetIn = A2;
const uint8_t SwitchOut = 12;
const uint8_t Sample = 13;
const uint8_t LEDpin = 11;

// Switch High, Sample Low: Channel 0 Out
// Switch High, Sample High: Channel 1 Out
// Switch Low: LPC goes through

DAC8554 dac = DAC8554(false);

unsigned long sum = 0;
unsigned long DACsum = 0;
unsigned int counter = 0;
unsigned int counterDAC = 0;
boolean state = false;

void setup() {
  pinMode(Sample, INPUT);
  pinMode(SwitchOut, INPUT);
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, LOW);
  dac.setDAC(0);
  dac.changeChannelBits(1,1,1,0,0,1,0,0);
  dac.setPins(3,2,4,7,6,5,8);
  dac.initializePins();
  dac.updateAllChannels(0);
}

void loop() {
  // If we are told to sample, we add up the input from the LPC
  if(digitalRead(Sample)) 
  {
    if(!digitalRead(SwitchOut)) // If its low, we are on LPC mode
    {
      if(!state)
      {
        state = true;
        sum = 0;
        DACsum = 0;
        counter = 0;  
        counterDAC = 0;
        digitalWrite(LEDpin, HIGH);
      }
      analogRead(LPCIn); // Dump first one due to multiplexer switching
      for(int i = 0; i < 50; i++)
      {
        sum = sum + analogRead(LPCIn);
        counter = counter + 1;
      }
      analogRead(DACSetIn);
      for(int i = 0; i < 50; i++)
      {
        DACsum = DACsum + analogRead(DACSetIn);
        counterDAC = counterDAC + 1;  
      }
      
    }
  }
  else // Once we are done, we set the DAC to the average measured.
  {
    if (state)
    {
      state = false;
      digitalWrite(LEDpin, LOW);
      // The DAC output is the average of the 
      // measured values, times 64 due to 6 bit
      // more resolution, rounded correctly. 
      double highVoltage = (double)(64*sum)/counter;
      double lowVoltage = (double)(64*DACsum)/counterDAC; 
      unsigned int avgHigh = (unsigned int)(highVoltage+0.5);
      unsigned int avgLow = (unsigned int)(lowVoltage+0.5);
      dac.updateChannel(channelHigh,avgHigh);
      dac.updateChannel(channelLow,avgLow);
    }
  }
}
