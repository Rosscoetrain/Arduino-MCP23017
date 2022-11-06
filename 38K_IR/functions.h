// function that executes whenever data is received from master
// this function is registered as an event, see setup()

void receiveEvent(int howMany) {
  int x = Wire.read();    // receive byte as an integer

  /*
    Serial.print("W ");  //print commented out as it caused 38k burbles
    Serial.println(x,HEX);
  */

  sw = 0;

}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()

void  requestEvent() {
  int cnt = 2;
  byte buff[2];

  buff[0] = irState1;
  buff[1] = irState2;

  /*
  if (sw > -3 ){            //print commented out as it caused 38k burbles
    Serial.print("buff ");
    Serial.print(buff[0],HEX);
    Serial.print(" ");
    Serial.println(buff[1],HEX);
   }
  sw++;
  */

#ifdef mon_out      
  if (mon_word & mon_bit ) {
    digitalWrite(mon_out,1);
  } else {
    digitalWrite(mon_out,0);
  }                         //DEBUG CODE to scope pgm status of one sensor
#endif

  Wire.write(buff, cnt);
  digitalWrite(13, 0);      //int turn off tell host we have new data
}
