#include <Wire.h>
#include <FrequencyTimer2.h>


#include "defines.h"
#include "variables.h"
#include "functions.h"



void setup() {
  Wire.begin(I2C_ADDRESS);                // join i2c bus with address I2C_ADDRESS
  Wire.onReceive(receiveEvent);           // register event toreceive data from i2c master (cmd and arg)
  Wire.onRequest(requestEvent);           // register event to send data to i2c master (irStatus )

  pinMode(PIN_DETECT1, INPUT_PULLUP);     //needs to be reversed
  myInput[1] = PIN_DETECT1;

  pinMode(PIN_DETECT2, INPUT_PULLUP);
  myInput[2] = PIN_DETECT2;

  pinMode(PIN_DETECT3, INPUT_PULLUP);
  myInput[3] = PIN_DETECT3;

  pinMode(PIN_DETECT4, INPUT_PULLUP);
  myInput[4] = PIN_DETECT4;

  pinMode(PIN_DETECT5, INPUT_PULLUP);
  myInput[5] = PIN_DETECT5;

  pinMode(PIN_DETECT6, INPUT_PULLUP);
  myInput[6] = PIN_DETECT6;

  pinMode(PIN_DETECT7, INPUT_PULLUP);
  myInput[7] = PIN_DETECT7;

  pinMode(PIN_DETECT8, INPUT_PULLUP);
  myInput[8] = PIN_DETECT8;

  pinMode(PIN_DETECT9, INPUT_PULLUP);
  myInput[9] = PIN_DETECT9;

  pinMode(PIN_DETECT10, INPUT_PULLUP);
  myInput[10] = PIN_DETECT10;

  pinMode(PIN_DETECT11, INPUT_PULLUP);
  myInput[11] = PIN_DETECT11;

  pinMode(PIN_DETECT12, INPUT_PULLUP);
  myInput[12] = PIN_DETECT12;

  pinMode(PIN_DETECT13, INPUT_PULLUP);
  myInput[13] = PIN_DETECT13;

  pinMode(PIN_DETECT14, INPUT_PULLUP);
  myInput[14] = PIN_DETECT14;

  pinMode(PIN_DETECT15, INPUT);
  myInput[15] = PIN_DETECT15;

  pinMode(PIN_DETECT16, INPUT);
  myInput[16] = PIN_DETECT16;

  state = 0;
  irState1 = 0;
  irState2 = 0;
  irOld1 = irState1;
  irOld2 = irState2;

  /*
    a4 and a5  used for i2c
    a4 is i2c sda
    a5 is i2c scl
    a6 and a7 are analog input only
    a7 wont be used for ir input but track pwr on (only read ir if track pwr on)
    D13  int data has changed. D13 is connected to open collector driver that inverts
         so D13 low means no int, D13 high means int active HAL will poll for data
  */

  pinMode(13, OUTPUT);
  digitalWrite(13, 0);                    //int data changed is neg active
  pinMode(PIN_IR, OUTPUT);
  FrequencyTimer2::setPeriod(26);         //generate 38k pulses on pin D11
  FrequencyTimer2::enable();

#ifdef mon_out
    pinMode(mon_out, OUTPUT);
    digitalWrite(mon_out,0);              //DEBUG CODE FOR pin defined by mon_in
#endif
    
  Serial.begin(115200);
  Serial.println("10 cnt to be on / pwr on gate/ i2c 20017 sim adr 8 vpin 300-315  315 also pwr on");
  Serial.println("Using FrequencyTimer2.h instead of tone and hal int pin 40");
}

int val;
int i;
int j;

                                          // following 3 used for non blocking delay

unsigned long newMillis = 0;
const long rdDelay = 20;
const long offDelay = 200;


/* 
    loop code uses a state machine defined by state
    state=0 Start new cycle
            turn on 38k
            init 20ms (rdDelay) timer
            state=1
    state=1 wait 20ms (non blocking)
            state=2
    state=2 read each input (all 16)
            repeat CNTS (10) times
            if sensor on for all 10 reads set sensor on else set sensor off
            turn off 38k
            init 200ms (offDelay) timer
            state=3
    state=3 wait 200ms (non blocking)
            if any sensor changed from previous cycle tell HAL to Poll (D13=1)
            state=0
*/

void loop() {

  val = (analogRead(A7) > 500) ? 0 : 1;   // a7 is track pwr on sig neg active
  if (val) {                              // only read sensors if track pwr on
    if (state == 0) {
      for ( i = 1; i <= 16; i++)
      {
        cntr[i] = 0;
      }

      FrequencyTimer2::enable();          //turn on 38khz
      newMillis = millis() + rdDelay;
      state = 1;
      //delay(20);
    }

    if (state == 1 && newMillis < millis() )  {
      state = 2;
    }
    if (state == 2) {
      for (  j = 0; j < CNTS; j++)
      {

        for ( i = 1; i <= 16; i++)
        {
          if (i == 1) {                   // reverse 300
            val = !digitalRead(PIN_DETECT1); //300 SIG INVERTED

          } else if (i > 14) {            // 15 and 16 are a6 and a7
            val = (analogRead(myInput[i]) > 500) ? 1 : 0;   // a6 A7 is not digital

          } else {
            val = digitalRead(myInput[i]);
          }
          if (val == 0) {
            cntr[i]++;
          } else {
            cntr[i] = 0;
          }
        }                                 // end of for i

      }                                   // end of for j

      for ( i = 1; i <= 8; i++)
      {
        if (cntr[i] == CNTS) {
          bitWrite(irState, i - 1, 0);
        } else {
          bitWrite(irState, i - 1, 1);
        }

      }
      irState1 = irState;
      for ( i = 9; i <= 16; i++)
      {
        if (cntr[i] == CNTS) {
          bitWrite(irState, i - 9, 0);
        } else {
          bitWrite(irState, i - 9, 1);
        }

      }
      irState2 = irState;

      FrequencyTimer2::disable();         // turn off 38khz
      newMillis = millis() + offDelay;
      state = 3;
      //delay(200);
    }
    if (state == 3 && newMillis < millis()) {
      state = 0;
    }
  } else {                                // pwr off no sensors active
    irState1 = 0xff;
    irState2 = 0xff;
  }
  if (irState1 != irOld1 || irState2 != irOld2 ) {
    irOld1 = irState1;
    irOld2 = irState2;
    digitalWrite(13, 1);                  //int tell host we have new data
  }
}
