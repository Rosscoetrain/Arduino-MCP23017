
int pb = 0;
int debug1 = 1;
byte state = 0;

int val;
byte pb_out;
byte pb_stat;
int last_b; 
byte pb_vals [ ] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f, 0xff};
int i;
int j;

int c;
int x = 0;
int dly = 100;
int dly2 = 200;

char cmd = '0';
byte arg;
byte arg2;

//servo vars
byte sclosed[9];
byte sthrown[9];
byte scenter[9];
byte sservo;
byte sread[3];
byte spending;

int spins [ ] = {3, 5, 6, 9, 10, 11, 13, 12};      // Servo pins
int rpins [ ] = {2, 4, 7, 8, A2, -30, A0, A1};   // relay output pins //-30 is flag this servo does not have relay output
//A3 used for i2c int data avail
byte t_stat[numservo];   //0 closed normal 1 thrown  2  init centered 3 request centered
int det [ ] = {0, 0, 0, 0, 0, 0, 0, 0, 0};  // if 0 detach after action, if 1 dont detach

Servo turnout[numservo];  // create servo object to control a servo
