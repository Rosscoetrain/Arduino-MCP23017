/*
   Arduino MCP23017 emulation for use with DCC-EX

   Based on original work by Mike B (Springlake on Discord)

   Most of this is just rewritten to make it easier to read and modify

   Rewrite by Ross (Rosscoe on Discord)   

 */


#include <Wire.h>
#include <EEPROM.h>
#include <Servo.h>

#include "defines.h"
#include "variables.h"
#include "functions.h"



void setup() {
  Wire.begin(I2C_ADDRESS);                // join i2c bus with address #18
//  digitalWrite(9,0);                    // int turn off tell host we have new data
  Wire.onReceive(receiveEvent);           // register event toreceive data from i2c master (cmd and arg)
  Wire.onRequest(requestEvent);           // register event to send data to i2c master (irStatus )  


  pinMode(A3, OUTPUT);
  digitalWrite(A3,0);                     //int data changed 0=no new data
  Serial.begin(115200);
  Serial.println("cs arduinio i2c ltc");
  pb_out = 0xff;                          //no buttons pushed
  pb_stat = pb_out; 

   
 
  // restore sclosed and sthrown from eeprom
  // adr 0 to adr NUMSERVO =ssclosed
  // adr NUMSERVO+1 toadr NUMSERVO+NUMSERVO =sthrown
  
  i = 0;
  while (i < NUMSERVO) {
    sclosed[i] = int(EEPROM[i]);
    i=i+1;
  }
  j=0;
  while (j < NUMSERVO) {
    sthrown[j] = int(EEPROM[i]);
    i=i+1;
    j=j+1;
  } 


  // first calculate center
  // then set to thrown (which first calls centerit which attaches then centers turnout)
  // then set to closed/normal
    
  for (i = 0; i < NUMSERVO; i++)  {
    scenter[i] = calc_center(sclosed[i], sthrown[i]);
   
    if ( i < NUMSERVO ) {
      if ( rpins[i] != -30 ) {            // -30 is flag this servo does not have relay output
        pinMode(rpins[i], OUTPUT);
        digitalWrite(rpins[i], HIGH);
      }
    }
  }
   
  //then set all servos to thrown

  for (i = 0; i < NUMSERVO; i++)  {

    Throw(i);                             // Set to thrown
    delay(200);
    t_stat[i] = 1;                        //show thrown
  }
  //then set all servos and relays to closed/normal
  state = 0x01;
  for (i = 0; i < NUMSERVO; i++)  {
    Close(i);                             // Set to closed/normal
    delay(300);
    if ( det[i] == 0 ) {
      turnout[i].detach();
      delay(200);
    }

    t_stat[i] = 0;                        //show closed/normal status

  }
  Serial.println("ltc Ready");
              
}



void loop() {
    
    pb = readButtons(A6);
    pb_out = pb_vals[pb];
    if (pb != 8) {
      Serial.print("pb=");
      Serial.print(pb);
      Serial.print(" pb_out ");
      Serial.println(pb_out,HEX);
    }

    if ( cmd != '0') {
       Serial.print("i2c cmd.");
       Serial.print(cmd);
       Serial.println(".");
       switch (cmd) {  
         case 'c':                        // cmd c center arg=servo lower case c
           Serial.print("center servo ");
           Serial.print(arg);
           Serial.print(" to value ");
           Serial.println(scenter[arg]);
           center_it(arg);
           t_stat[arg] = 3;               // show center requested
           cmd = '0';
           break;
         case 'E':                        // cmd E wrt eeprom

           // eepromcode goes here
           // write sclosed and sthrown to eeprom
           // adr 0 to adr numsevo =sclosed
           // adr NUMSERVO+1 toadr NUMSERVO+NUMSERVO =sthrown
           // update only writes if contents different than data

           i = 0;
           while (i < NUMSERVO) {
             EEPROM.update(i, byte(sclosed[i]));
             Serial.print(i);
             Serial.print(" i sclosed[i] ");
             Serial.println(sclosed[i]);
             i=i+1;
           }
           j=0;
           while (j < NUMSERVO) {
             EEPROM.update(i, byte(sthrown[j]));
             Serial.print(i);
             Serial.print(" j sthrown[j] ");
             Serial.print(sthrown[j]);
             Serial.print(" i ");
             Serial.println(i);     
             i=i+1;
            j=j+1;
           }  
           
           cmd = '0';
           break;
         case 'C':                        // cmd C wrt closed upper case C
           Serial.print("closed servo ");
           Serial.print(arg);
           Serial.print(" to value ");
           Serial.println(arg2);
           sclosed[arg] = arg2;
           scenter[arg] = calc_center(sclosed[arg], sthrown[arg]);  //recalc center           
           cmd = '0'; 
           break;       
         case 'T':                        // cmd T wrt thrown upper case T
           Serial.print("thrown servo ");
           Serial.print(arg);
           Serial.print(" to value ");
           Serial.println(arg2);
           sthrown[arg] = arg2;
           scenter[arg] = calc_center(sclosed[arg], sthrown[arg]);      //recalc center           
           cmd = '0'; 
           break;  
         case 'S':                        // cmd S set turnouts closed or thrown upper case S
           state = 0x01;
           for (i = 0; i < NUMSERVO; i++)  {
          
             if ( ((arg & state) > 0) && (t_stat[i]!= 0) ) {            // t_stat=0 closed
               Serial.print("T");
               Serial.print(i);
               Serial.print(" closed value ");
               Serial.println(sclosed[i]); 
               Close(i);                  // actually close turnout and update t_stat
             } else {
               if (((arg & state) ==0) && (t_stat[i] != 1)) {           // t_stat=1 thrown
                 Serial.print("T");
                 Serial.print(i);
                 Serial.print(" thrown value ");
                 Serial.println(sthrown[i]);
                 Throw(i);                // actually throw turnout and update t_stat
               }  
             }
            Serial.print("T");
            Serial.print(i);
            Serial.print(" state ");
            Serial.print(state,HEX);
            Serial.print(" arg ");
            Serial.print(arg,HEX);
            Serial.print(" a&s ");
            Serial.print((arg & state) > 0);
            Serial.print(" a&s1 ");
            Serial.print( (arg & state) ==0 );            
            Serial.print(" t_stat ");
            Serial.println(t_stat[i]);
            state = state << 1;
          }

           cmd = '0'; 
           break;                
         default:
            Serial.println("invalid i2c cmd");
      }     
    }

    if (pb_out != pb_stat) {   
      Serial.print("spending=");
      Serial.print(spending);      
      Serial.print(" pb_out=");
      Serial.print(pb_out); 
      Serial.print(" pb_stat=");
      Serial.print(pb_stat); 
      Serial.print(" pb=");
      Serial.println(pb); 
      pb_stat = pb_out; 
      digitalWrite(A3,1);                 // int data changed 1 = new data available
      Serial.println("a3 1");
    }
 
   
}
