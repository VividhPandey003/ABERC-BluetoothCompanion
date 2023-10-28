#include <AFMotor.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "eyes.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SoftwareSerial bluetoothSerial(9, 10); // RX, TX
AF_DCMotor motor3(3, MOTOR34_1KHZ);    // initial motors pin
AF_DCMotor motor4(4, MOTOR34_1KHZ);
char command;

void setup() {
  bluetoothSerial.begin(9600); // Set the baud rate to your Bluetooth module
  Serial.begin(9600);
  Serial.println("Booting");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {
    }
  }
  display.clearDisplay();
  display.display();
}

unsigned char readkey(char input) {
  unsigned char ret = 0;
  if (input == 'm') {
    ret = 1;
  } else if (input == 'l') {
    ret += 2;
  } else if (input == 'r') {
    ret += 4;
  }

  return ret;
}

int xp = 16;
int mood = 0;

void forward() {
  motor3.setSpeed(255); // Define maximum velocity
  motor3.run(FORWARD);  // rotate the motor clockwise
  motor4.setSpeed(255); // Define maximum velocity
  motor4.run(FORWARD);  // rotate the motor clockwise
}

void back() {
  motor3.setSpeed(255); // Define maximum velocity
  motor3.run(BACKWARD); // rotate the motor anti-clockwise
  motor4.setSpeed(255); // Define maximum velocity
  motor4.run(BACKWARD); // rotate the motor anti-clockwise
}

void left() {
  motor3.setSpeed(255); // Define maximum velocity
  motor3.run(BACKWARD);  // rotate the motor clockwise
  motor4.setSpeed(255); // Define maximum velocity
  motor4.run(FORWARD);  // rotate the motor clockwise
}

void right() {
  motor3.setSpeed(255); // Define maximum velocity
  motor3.run(FORWARD); // rotate the motor anti-clockwise
  motor4.setSpeed(255); // Define maximum velocity
  motor4.run(BACKWARD); // rotate the motor anti-clockwise
}

void topleft() {
  motor3.setSpeed(180); // Define maximum velocity
  motor3.run(FORWARD);  // rotate the motor clockwise
  motor4.setSpeed(255); // Define maximum velocity
  motor4.run(FORWARD);  // rotate the motor clockwise
}

void topright() {
  motor3.setSpeed(255); // Define maximum velocity
  motor3.run(FORWARD);  // rotate the motor clockwise
  motor4.setSpeed(180); // Define maximum velocity
  motor4.run(FORWARD);  // rotate the motor clockwise
}

void bottomleft() {
  motor3.setSpeed(180); // Define maximum velocity
  motor3.run(BACKWARD); // rotate the motor anti-clockwise
  motor4.setSpeed(255); // Define maximum velocity
  motor4.run(BACKWARD); // rotate the motor anti-clockwise
}

void bottomright() {
  motor3.setSpeed(255); // Define maximum velocity
  motor3.run(BACKWARD); // rotate the motor anti-clockwise
  motor4.setSpeed(180); // Define maximum velocity
  motor4.run(BACKWARD); // rotate the motor anti-clockwise
}

void Stop() {
  motor3.setSpeed(0);  // Define minimum velocity
  motor3.run(RELEASE); // stop the motor when releasing the button
  motor4.setSpeed(0);  // Define minimum velocity
  motor4.run(RELEASE); // stop the motor when releasing the button
}

void loop() {
  int n;
  if (bluetoothSerial.available() > 0) {
    command = bluetoothSerial.read();
    Stop(); // initialize with motors stopped
    Serial.println(command);
    switch (command) {
      case 'F':
        forward();
        break;
      case 'B':
        back();
        break;
      case 'L':
        left();
        break;
      case 'R':
        right();
        break;
      case 'G':
        topleft();
        break;
      case 'I':
        topright();
        break;
      case 'J':
        bottomright();
        break;
      case 'H':
        bottomleft();
        break;
    }

    static int xd = 4;
    static int wait = 0;
    static int step = 0;
    int x1, x2;
    if (wait > 0) {
      wait--;
      delay(1);
    } else {
      x1 = xd + (xp > 16 ? (16 + 2 * (xp - 16)) : xp);
      x2 = 64 + xd + (xp < 16 ? (-16 + (xp * 2)) : xp);
      switch (step) {
        case 0:
          display.clearDisplay();
          if (xp < 6) { // if eyes to the left
            display.drawBitmap(x1, 18, peyes[mood][2][0], 32, 32, WHITE);
            display.drawBitmap(x2, 18, peyes[mood][1][1], 32, 32, WHITE);
          } else if (xp < 26) { // if eyes to the right
            display.drawBitmap(x1, 18, peyes[mood][0][0], 32, 32, WHITE);
            display.drawBitmap(x2, 18, peyes[mood][0][1], 32, 32, WHITE);
          } else {   // if normal
            display.drawBitmap(x1, 18, peyes[mood][1][0], 32, 32, WHITE);
            display.drawBitmap(x2, 18, peyes[mood][2][1], 32, 32, WHITE);
          }
          display.display();
          wait = random(250, 1000);
          n = random(0, 7);
          if (n == 6) {
            step = 1;
          } else {
            step = 2;
          }
          break;
                case 1:
          display.clearDisplay();
          display.drawBitmap(x1, 8, eye0, 32, 32, WHITE);
          display.drawBitmap(x2, 8, eye0, 32, 32, WHITE);
          display.display();
          wait = 100;
          step = 0;
          break;
        case 2:
          n = random(0, 10);
          if (n < 5) xd--;
          if (n >= 5) xd++;
          if (xd < -4) xd = -3;
          if (xd > 4) xd = 3;
          wait = 0;
          step = 0;
          break;
      }
    }

    if (command == 'L') n = readkey('l');
    if (command == 'R') n = readkey('r');
    if (command == 'W') n = readkey('m');
    if (n == 2) xp = (xp <= 0 ? 0 : xp - 1);
    if (n == 4) xp = (xp >= 32 ? 32 : xp + 1);
    if (command == 'W') {
      mood = (mood >= 5 ? 0 : mood + 1);
    }
    if (n != 0) {
      wait = 0;
      step = 0;
    }
  }
}

