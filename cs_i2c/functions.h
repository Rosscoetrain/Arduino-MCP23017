int readButtons(int pin)
// returns the button number pressed, or 8 for none pressed
// int pin is the analog pin number to read
{

  int b, c = 0;
  int c1, c2, c3;
 
  c = analogRead(pin); // get the analog value   

  c1 = c;
  delay(10);
  c = analogRead(pin); // get the analog value   

  c2 = c;
  delay(10);
  c3 = analogRead(pin); // get the analog value   
  c = c1;
  if (c2 < c1) {
    c = c2;
  }
  if (c3 < c) {
    c = c3;
  }

  if ( abs(c1 - c2) > 30 || abs(c2 - c3) > 30 || abs(c3 - c1) > 30 ) {
    c = 2048;
  } else {
    c=int(c1+c2+c3)/3;
  } 
  if ((debug1 == 1) && (c < 1000 )) {
    Serial.print("raw1 ");
    Serial.print(c1);
    Serial.print(" raw2 ");
    Serial.print(c2);

    Serial.print(" raw3 ");
    Serial.print(c3);
    Serial.print(" use ");
    Serial.println(c);
  }

  
   
  if (c > 970)
  {

    b = 8; // buttons have not been pressed

  }   else

    if (c > 873 && c < 970)

    {

      b = 0; // button 0 pressed

    }     else

      if (c < 873 && c > 791)

      {

        b = 1; // button 1 pressed

      }       else

        if (c > 711 && c < 791)

        {

          b = 2; // button 2 pressed

        }         else

          if (c > 621 && c < 711)

          {

            b = 3; // button 3 pressed

          }           else

            if (c > 510 && c < 621)

            {

              b = 4; // button 4 pressed

            }     else

              if (c > 360 && c < 510)

              {

                b = 5; // button 5 pressed

              }    else

                if (c > 136 && c < 360)

                {

                  b = 6; // button 6 pressed

                }
  if (c < 136 )

  {

    b = 7; // button 7 pressed

  }
/*
    if (c < 980) { //button pushed
      Serial.print(" c=");
      Serial.println(c);
      if (c >= 0 && c <= 350) {
        Serial.println("pb0");
        b=0;
      }
      if (c > 350 && c <= 650) {
        Serial.println("pb1");
        b=1;
      }
      if (c > 650 && c <= 750) {
        Serial.println("pb2");
        b=2;
      }
      if (c > 750 && c <= 850) {
        Serial.println("pb3");
        b=3;
      }
      if (c > 850 && c <= 925) {
        Serial.println("pb4");
        b=4;
      }
      if (c > 925 && c <= 979) {
        Serial.println("pb5");
        b=5;
      }
      last_b=b;

    } else {  //button not pushed
      b=8;
    }
    
    if ((last_b != b)&& (b !=8)) {
      Serial.print("b=");
      Serial.print(b);
      Serial.print("last b=");
      Serial.println(last_b);
    }
    */
    if ( b!=8){
      Serial.print(" read b=");
      Serial.println(b);
    }
  return b;
}


void center_it(int i)    //attach and center servo i
{
  if (!turnout[i].attached() ) {
    turnout[i].attach(spins[i]);
    delay(dly);
  }
  turnout[i].write(scenter[i]);  //move halfway
  delay(3 * dly);

}


byte  calc_center(byte sclosed,byte sthrown){
  byte c;
 
   if (sclosed < sthrown) {
        c = sthrown - sclosed;
        c = c / 2;
        c = c + sclosed;
    } else {
      c = sclosed - sthrown;
      c = c / 2;
      c = c + sthrown;
    }

    return c;
}


void Throw( int i)
{
  center_it(i);  //attach and center servo i
  if (sthrown[i] >= scenter[i]) {
  
    x = sthrown[i] + 2;
  } else {
    x = sthrown[i] - 2;
  }
  turnout[i].write(x);
  delay(dly);
  turnout[i].write(sthrown[i]);            // Set to thrown
  delay(dly);
  Serial.print(t_stat[i]);
  t_stat[i] = 1;


  if ( i < numservo ) {
    if ( rpins[i] != -30 ) {         // -30is flag this servo doesnot have relay output
      digitalWrite(rpins[i], LOW);

    }
  }
  if ( det[i] == 0 ) {
    turnout[i].detach();
    delay(dly);
  }
}

void Close(int i)
{
  center_it(i);  //attach and center servo i
  if (sclosed[i] <= scenter[i])
  {
    x = sclosed[i] - 2;
  } else {
    x = sclosed[i] + 2;
  }
  turnout[i].write(x);
  delay(dly);
  turnout[i].write(sclosed[i]);            // Set to closed
  delay(dly);
  if ( i < numservo ) {
    if ( rpins[i] != -30 ) {         // -30is flag this servo doesnot have relay output
      digitalWrite(rpins[i], HIGH);
    }
  }
  Serial.print(t_stat[i]);
  t_stat[i] = 0;
  if ( det[i] == 0 ) {
    turnout[i].detach();
    delay(dly);
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  byte x;
  byte buff[9];
 if (howMany == 1) {
   x = Wire.read();
   if ( x != 0x12 ) { 
    Serial.println(x,HEX); 
   }
 } else {
    Serial.print(howMany);
    Serial.print("W ");
    for (int a = 0; a < howMany; a++) {
      if (a < 9) {
        buff[a] = Wire.read(); 
        Serial.print(buff[a],HEX);
        Serial.print(" ");
        spending=0;  //flag to say read pb not servo 
      } else {
        x = Wire.read();   
      }
    }
    Serial.println(" w");    
 }
 if ((buff[0] == 0x12) && (howMany == 3)) {  //T CMD TO CLOSE THROW TURNOUT
    cmd = 'S';   //uper caseS set turnouts closed or open
    arg = buff[2]; // turnout byte encoded

    buff[1] = 0; // reset so single 12 wont cause loop        
 }
 if ((buff[0] == 0xc0) ) {  //sending cmds  dlim=0xC0 buff[0] cmd buff[1] data buff[2] servo(0-9) buff[3]
   if ( (buff[1] == 0x10) || (buff[1] == 0x11)  ) {   //cmd wrt closed or thrown  
     if  (buff[1] == 0x10) {  //wrt closed
       cmd = 'C';   //uper caseC wrt closed
       arg = buff[3]; // servo
       arg2 = buff[2]; //data to wrt
     }
     if  (buff[1] == 0x11) {  //wrt thrown
       cmd = 'T';   //uper caseT wrt thrown
       arg = buff[3]; // servo
       arg2 = buff[2]; //data to wrt      
     }

   }
   if (  buff[1] == 0x20 ) {  // cmd asking to send back close throw center for servo(buff[2] 
     sservo = buff[2];
     spending=1;  //flag to say read servo data not pb
     sread[0] = sclosed[sservo];
     sread[1] = sthrown[sservo];
     sread[2] = scenter[sservo];
   }
   if (  buff[1] == 0x21 ) {  // cmd to write sclosed and sthrown into EEPROM
       cmd = 'E';    //write closed and thrown values to eeprom
   }
   if (  buff[1] == 0x22 ) {  //center servo
      cmd = 'c';    //center arg=servo lower case c
      arg = buff[2];

   }
 }
} 
// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void  requestEvent()
{
  int cnt = 2;
  byte buff[2];
    
    buff[0] = pb_out;
    buff[1] = 0xff;
 
//       Serial.print("buff ");
 
//       Serial.println(buff[0],HEX);
     
     
   if (spending == 1) {
     Wire.write(sread, 3);
     spending = 0;
   } else {
     Wire.write(buff, cnt);
     digitalWrite(A3,0);   //int data changed 0=no new data
     Serial.println("a3 0");
   }  
    
}
