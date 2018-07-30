/*****************************************************************
  XBee_Serial_Passthrough.ino

  Set up a software serial port to pass data between an XBee Shield
  and the serial monitor.

  Hardware Hookup:
  The XBee Shield makes all of the connections you'll need
  between Arduino and XBee. If you have the shield make
  sure the SWITCH IS IN THE "DLINE" POSITION. That will connect
  the XBee's DOUT and DIN pins to Arduino pins 2 and 3.

*****************************************************************/
// We'll use SoftwareSerial to communicate with the XBee:

bool connected = false;
const int heartBeat = 1000; // ms receiver must be alive after heartBeat ms
unsigned long lastTime = -1;
unsigned long currentTime = 0;

#define up_button  8
#define down_button A1
#define left_button 9
#define right_button  12

#define stick_button  A0
#define level_stick A2
#define vertical_stick  A3

int left_state;
int right_state;
int up_state;
int down_state;

int stick_state;

/*int repetitions = 1;
  // average the analog readings //
  float lv = 0.;
  float vv = 0.;
  for (int i = 0 ; i < repetitions; ++i)
  {
  lv +=  analogRead (level_stick);
  vv += analogRead (vertical_stick);
  }
  int level_value = lv / repetitions;
  int vertical_value = vv / repetitions;*/
int level_value ;
int vertical_value;

#include <stdarg.h>
void p(char *fmt, ... ) {
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  Serial.print(buf);
}
typedef struct {
  int left_state;
  int right_state;
  int up_state;
  int down_state;
  int stick_state;
  int level_value;
  int vertical_value;
  char final_char;
} joystick_t;
joystick_t joystick;
const int msglength = sizeof(joystick);
void setup()
{

  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode (left_button, INPUT);
  pinMode (right_button, INPUT);
  pinMode (up_button, INPUT);
  pinMode (down_button, INPUT);

  pinMode (stick_button  , INPUT);
  pinMode (level_stick   , INPUT);
  pinMode (vertical_stick, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, connected);
}
void connect() {
  while (!connected)
  {
    if (Serial.available()) {
      char tmp = Serial.read();
      if (tmp == 'c')
      {
        Serial.write('o');
        connected = true;
        while (Serial.available()) Serial.read(); /// empty the serial buffer
      }
    }
  }
  // handshake
  Serial.write('a');
  int counter = 0;
  while (!Serial.available() && counter < 1000)
  {
    counter++;
    delay(10);
  }
  if (Serial.available())
  {
    char tmp = Serial.read();
    if (tmp == 'b') {
      connected = true;
    } else {
      connected = false;
    }
  } else {
    connected = false;
  }




}
void loop()
{
  connect();
  digitalWrite(13, connected);
  if (connected)
  {
    Serial.write('#');
    // THis is the client. Send the status!
    joystick.left_state = digitalRead (left_button);
    joystick.right_state = digitalRead (right_button);
    joystick.up_state = digitalRead (up_button);
    joystick.down_state = digitalRead (down_button);
    joystick.stick_state = digitalRead (stick_button);
    joystick.level_value = analogRead (level_stick);
    joystick.vertical_value = analogRead (vertical_stick);
    joystick.final_char = 'f';
    char message[msglength];
    memcpy(message, &joystick, msglength);
    for (int ii = 0; ii < msglength; ++ii)
    {
      Serial.write(message[ii]);
    }
  }
}
