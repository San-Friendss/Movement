// Blynk App
#define BLYNK_AUTH_TOKEN "l4chGUuJL7Yso2jt3yPCQ377vewUzICt"
#define BLYNK_AUTH_TOKEN_WATER_PUMP "0BVxluy8unor3WZHVLYNys90EzRM5IZi"
#define BLYNK_PRINT Serial

// include the libraries
#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// define the pins for the motor driver
#define LED 2
#define LeftForward 19
#define LeftBackward 18
#define RightForward 5
#define RightBackward 17

// create servo object to control a servo
Servo servo;

// variable to store the range in cm
int RangeInCentimeters, leftDistance, rightDistance, IR_R, IR_L;

// 0 is manual mode, 1 is auto mode
int mode = 0;

// Manual Mode
bool forward = 0;
bool backward = 0;
bool left = 0;
bool right = 0;

// initial position of the joystick
int x = 50;
int y = 50;

// Your WiFi credentials.
char ssid[] = "apkmew";
char pass[] = "6410500360";

WidgetBridge bridge(V0);

// move functions
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void stop();
void manualJoystick();
void changePath();
void compareDistance();
void autoCar();

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0) // change mode
{
    mode = param.asInt();
    bridge.virtualWrite(V2, mode);
}

// Get the ultrasonic values from the Blynk app
BLYNK_WRITE(V5) 
{
    RangeInCentimeters = param.asInt();
}

// Get the IR Left values from the Blynk app
BLYNK_WRITE(V6) 
{
    IR_L = param.asInt();
}

// Get the IR Right values from the Blynk app
BLYNK_WRITE(V7) 
{
    IR_R = param.asInt();
}

// Get the joystick values x
BLYNK_WRITE(V8)
{
    x = param[0].asInt();
}

// Get the joystick values y
BLYNK_WRITE(V9)
{
    y = param[0].asInt();
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
    // Change Web Link Button message to "Congratulations!"
    Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
    Blynk.setProperty(V3, "onImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
    Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
    bridge.setAuthToken(BLYNK_AUTH_TOKEN_WATER_PUMP);
    digitalWrite(LED, HIGH);
}

void setup()
{
    Serial.begin(9600);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.iot-cm.com", 8080); // Blynk
    pinMode(LED, OUTPUT);                                                // LED
    pinMode(LeftForward, OUTPUT);                                        // Left Forward
    pinMode(LeftBackward, OUTPUT);                                       // Left Backward
    pinMode(RightForward, OUTPUT);                                       // Right Forward
    pinMode(RightBackward, OUTPUT);                                      // Right Backward
    servo.attach(21);                                                    // Servo Pin
    servo.write(90);                                                     // Servo Initial Position
    stop();                                                              // Stop the car
}

void loop()
{
    Blynk.run();                            // Blynk
    if (mode == 1) // auto mode
    {
        autoCar();
    }
    else if (mode == 0) // manual mode
    {
        manualJoystick();
    }
}

void stop()
{
    digitalWrite(19, LOW); // Left Forward
    digitalWrite(18, LOW); // Left Backward
    digitalWrite(5, LOW); // Right Forward
    digitalWrite(17, LOW); // Right Backward
}

void moveForward()
{
    digitalWrite(19, LOW); // L Backward
    digitalWrite(18, HIGH); // L Forward 
    digitalWrite(5, HIGH); // R Forward
    digitalWrite(17, LOW); // R Backward
}

void moveBackward()
{
    digitalWrite(19, HIGH); // L Backward
    digitalWrite(18, LOW); // L Forward 
    digitalWrite(5, LOW); // R Forward
    digitalWrite(17, HIGH); // R Backward
}

void turnLeft()
{
    digitalWrite(19, LOW); // L Backward
    digitalWrite(18, LOW); // L Forward 
    digitalWrite(5, HIGH); // R Forward
    digitalWrite(17, LOW); // R Backward
}

void turnRight()
{
    digitalWrite(19, LOW); // L Backward
    digitalWrite(18, HIGH); // L Forward 
    digitalWrite(5, LOW); // R Forward
    digitalWrite(17, LOW); // R Backward
}

void changePath() // change path if there is an obstacle
{
    stop();
    delay(250);
    moveBackward();
    delay(800);
    stop();
    delay(250);

    servo.write(35); // check distance to the right
    delay(500);
    rightDistance = RangeInCentimeters; // set right distance
    delay(500);

    servo.write(145); // check distance to the left
    delay(500);
    leftDistance = RangeInCentimeters; // set left distance
    delay(500);

    servo.write(90); // return to center
    delay(100);

    compareDistance(); // compare the distances
}

void compareDistance() // find the less obstructed path
{
    if (leftDistance > rightDistance) // left is less obstructed
    {
        turnLeft();
        delay(100);
    }
    else if (rightDistance > leftDistance) // right is less obstructed
    {
        turnRight();
        delay(100);
    }
    else // if bot distances are equal
    {
        turnRight();
        delay(300);
    }
}

void autoCar() // auto mode
{
    if ((RangeInCentimeters < 30) || (digitalRead(IR_R) == 0) || (digitalRead(IR_L) == 0))
    {
        changePath();
        delay(500);
    }
    else
    {
        moveForward();
    }
}

void manualJoystick() // manual mode with joystick
{
    if (y > 70)
    {
        moveForward();
    }
    else if (y < 30)
    {
        moveBackward();
    }
    else if (x < 30)
    {
        turnLeft();
    }
    else if (x > 70)
    {
        turnRight();
    }
    else if (x < 70 && x > 30 && y < 70 && y > 30)
    {
        stop();
    }
}