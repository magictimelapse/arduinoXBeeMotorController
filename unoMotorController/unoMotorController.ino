
#include <Servo.h>
// Serial's DOUT (TX) is connected to pin 2 (Arduino's Software RX)
// Serial's DIN (RX) is connected to pin 3 (Arduino's Software TX)
//SoftwareSerial Serial(2, 3); // RX, TX
// Serial: Serial

int pwm_a = 6;  //PWM control for motor outputs 1 and 2
int pwm_b = 9;  //PWM control for motor outputs 3 and 4
int dir_a = 7;  //direction control for motor outputs 1 and 2
int dir_b = 8;  //direction control for motor outputs 3 and 4

#define DEBUG false
bool connected = false;
/*const int heartBeat = 1000; // ms receiver must be alive after heartBeat ms
  unsigned long lastTime = -1;
  unsigned long currentTime = 0;
  int left_state = 0;
  int right_state = 0;
  int up_state = 0;
  int down_state = 0;
  int stick_state = 0;
  int level_value = 512;
  int vertical_value = 512;
  int vertical_value_deg = 90;*/
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
long last = 0;
Servo servo;
const int minAngle = 30;
void setup()
{
  // Set up both ports at 9600 baud. This value is most important
  // for the Serial. Make sure the baud rate matches the config
  // setting of your Serial.
  
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, connected);
  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(dir_a, OUTPUT);
  pinMode(pwm_b, OUTPUT);

  pinMode(dir_b, OUTPUT);

  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
  servo.attach(9);
  int vertical_value = 512;
  int vertical_value_deg = map(vertical_value, 0, 1023, 0 + minAngle, 180 - minAngle);
  servo.write(vertical_value_deg);
}
void connect()
{


  while (!connected)
  {

    digitalWrite(13, connected);
    //empty serial buffer //
    while (Serial.available())
    {
      char tmp = Serial.read();


    }

    Serial.write('c');
    delay(500);

    if (Serial.available()) {
      char tmp = Serial.read();


      if (tmp == 'o') {

        connected = true;
        delay(500);
      }
    }
  }


  int counter = 0;
  while (!Serial.available() && counter < 100)
  {
    counter++;
    delay(50);
  }
  if (Serial.available())
  {
    char tmp = Serial.read();
    if (tmp == 'a') {
      connected = true;
      Serial.write('b');

    } else {
      connected = false;
    }
  } else {
    connected = false;
  }
  //if (!connected)

}
void loop()
{
  last = millis();
  connect();

  digitalWrite(13, connected);
  if (connected)
  {
    //This is the server, so it needs to wait for a command
    int counter = 0;
    int maxCounter = 1000;
    while (!Serial.available() && counter < maxCounter)
    {
      counter++;
      delay(10);
    }
    if (counter == maxCounter - 1)
    {
      connected = false;
      goto end;
    }
    if (Serial.available())
    { // If data comes in from Serial, send it out to serial monitor
      char tmp = Serial.read();
      if (tmp == '#') {
        char message[msglength];
        for (int ii = 0; ii < msglength; ++ii)
        {
          while (!Serial.available()) {}; // todo timeout and disconnect
          message[ii] = Serial.read();
        }
        joystick.final_char = 'a'; // wrong char needs to be overwritten;
        memcpy(&joystick, message, msglength);
        /*up_state = joystick.up_state;
        down_state = joystick.down_state;
        left_state = joystick.left_state;
        right_state = joystick.right_state;
        vertical_value = joystick.vertical_value;
        level_value = joystick.level_value;
        stick_state = joystick.stick_state;*/
        char final_char = joystick.final_char;
        if (final_char != 'f') {
          connected = false;

        } else {
          connected = true;
          int vertical_value_deg = map(joystick.vertical_value, 0, 1023, 0 + minAngle, 180 - minAngle);
          servo.write(vertical_value_deg);
          if (joystick.up_state == 0) {
            digitalWrite(dir_a, HIGH);
            digitalWrite(dir_b, HIGH);
            analogWrite(pwm_a, 255);
            analogWrite(pwm_b, 255);
          } else if (joystick.down_state == 0) {
            digitalWrite(dir_a, LOW);
            digitalWrite(dir_b, LOW);
            analogWrite(pwm_a, 255);
            analogWrite(pwm_b, 255);

          } else {
            analogWrite(pwm_a, 0);
            analogWrite(pwm_b, 0);
          }

        }

      }
    }
end:
    delay(1);
  }
}
