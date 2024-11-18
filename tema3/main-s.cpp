#include <Arduino.h>
#include <SPI.h>

// which button has been pressed
#define BTN_1 1
#define BTN_2 2
#define BTN_3 3
#define BTN_START 4
#define BTN_5 5
#define BTN_6 6
#define BTN_7 7
#define NO_BTN -1

// led values
#define LED_1 1
#define LED_2 2
#define LED_3 3
#define LED_4 4
#define LED_5 5
#define LED_6 6

// player 1 RGB LED
#define RED_PIN_1 9
#define GREEN_PIN_1 8
#define BLUE_PIN_1 7

// player 2 RGB LED
#define RED_PIN_2 6
#define GREEN_PIN_2 5
#define BLUE_PIN_2 4

// Multiple Pushbuttons pin
#define MULTIPLE_BUTTONS A0

int analogValue;

// maximum analog values for each button
// player 1 buttons
#define BUTTON_1 270
#define BUTTON_2 370
#define BUTTON_3 530
// start button
#define BUTTON_START 650
// player 2 buttons
#define BUTTON_5 780
#define BUTTON_6 900
#define BUTTON_7 1023
// no button pressed
#define NO_BUTTON_PRESSED 100

// debounce buttons
volatile int confirmedButton = -1;
volatile int lastButton = -1;
volatile unsigned long lastDebounce = 0;
const int debounceDelay = 50;
volatile int currentButton = -1;

// SPI variables
volatile byte Slavesend; // data sent to master
int randomLed;           // data from the master

// variables for master interrupt after pressing a button
#define masterInterruptPin 3
int pinChangeInterruptState = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(RED_PIN_1, OUTPUT);
  pinMode(RED_PIN_2, OUTPUT);
  pinMode(GREEN_PIN_1, OUTPUT);
  pinMode(GREEN_PIN_2, OUTPUT);
  pinMode(BLUE_PIN_1, OUTPUT);
  pinMode(BLUE_PIN_2, OUTPUT);

  digitalWrite(BLUE_PIN_1, LOW);
  digitalWrite(RED_PIN_1, LOW);
  digitalWrite(GREEN_PIN_1, LOW);
  digitalWrite(BLUE_PIN_2, LOW);
  digitalWrite(RED_PIN_2, LOW);
  digitalWrite(GREEN_PIN_2, LOW);

  pinMode(masterInterruptPin, OUTPUT); // master interrupt pin set as output
  pinMode(MISO, OUTPUT);
  SPCR |= _BV(SPE);
  SPCR &= ~(_BV(MSTR)); // Turn on SPI in Slave Mode

  SPI.attachInterrupt();
}

// value of the next led to be on
ISR(SPI_STC_vect) // Interrupt routine function
{
  randomLed = SPDR; // Value received from master
}

void loop()
{
  // determining which button was pressed based on the analog value
  analogValue = analogRead(MULTIPLE_BUTTONS);
  currentButton = NO_BTN;
  if (analogValue > NO_BUTTON_PRESSED)
  {
    if (analogValue < BUTTON_1)
    {
      currentButton = BTN_1;
    }
    else if (analogValue < BUTTON_2)
    {
      currentButton = BTN_2;
    }
    else if (analogValue < BUTTON_3)
    {
      currentButton = BTN_3;
    }
    else if (analogValue < BUTTON_START)
    {
      currentButton = BTN_START;
    }
    else if (analogValue < BUTTON_5)
    {
      currentButton = BTN_5;
    }
    else if (analogValue < BUTTON_6)
    {
      currentButton = BTN_6;
    }
    else
      currentButton = BTN_7;
  }

  // debounce check, if the last pressed button is different from the current one
  if (currentButton != lastButton)
  {
    lastDebounce = millis();
    lastButton = currentButton;
  }

  // if the time difference between the last debounce check and the current time is greater
  // than the debounce delay, then the button has been pressed
  if ((millis() - lastDebounce) > debounceDelay)
  {
    if (currentButton != confirmedButton)
    {
      confirmedButton = currentButton;

      if (confirmedButton > NO_BTN)
      {
        // Serial.println(confirmedButton);
        SPDR = confirmedButton; // data to be sent to the master
        /* changing the state of the pin to activate an interrupt for the master,
        using the masterInterruptpin and the pinChangeInterruptState. */
        pinChangeInterruptState = !pinChangeInterruptState;
        digitalWrite(masterInterruptPin, pinChangeInterruptState);
      }
    }
  }

  // turning on the LED based on the data from the master
  if (randomLed == LED_1)
  {
    digitalWrite(RED_PIN_1, HIGH);
    digitalWrite(GREEN_PIN_1, LOW);
    digitalWrite(BLUE_PIN_1, LOW);
    digitalWrite(GREEN_PIN_2, LOW);
    digitalWrite(RED_PIN_2, LOW);
    digitalWrite(BLUE_PIN_2, LOW);
  }
  else if (randomLed == LED_2)
  {
    digitalWrite(GREEN_PIN_1, HIGH);
    digitalWrite(RED_PIN_1, LOW);
    digitalWrite(BLUE_PIN_1, LOW);
    digitalWrite(GREEN_PIN_2, LOW);
    digitalWrite(RED_PIN_2, LOW);
    digitalWrite(BLUE_PIN_2, LOW);
  }
  else if (randomLed == LED_3)
  {
    digitalWrite(BLUE_PIN_1, HIGH);
    digitalWrite(RED_PIN_1, LOW);
    digitalWrite(GREEN_PIN_1, LOW);
    digitalWrite(GREEN_PIN_2, LOW);
    digitalWrite(RED_PIN_2, LOW);
    digitalWrite(BLUE_PIN_2, LOW);
  }
  else if (randomLed == LED_4)
  {
    digitalWrite(RED_PIN_2, HIGH);
    digitalWrite(GREEN_PIN_2, LOW);
    digitalWrite(BLUE_PIN_2, LOW);
    digitalWrite(BLUE_PIN_1, LOW);
    digitalWrite(RED_PIN_1, LOW);
    digitalWrite(GREEN_PIN_1, LOW);
  }
  else if (randomLed == LED_5)
  {
    digitalWrite(GREEN_PIN_2, HIGH);
    digitalWrite(RED_PIN_2, LOW);
    digitalWrite(BLUE_PIN_2, LOW);
    digitalWrite(BLUE_PIN_1, LOW);
    digitalWrite(RED_PIN_1, LOW);
    digitalWrite(GREEN_PIN_1, LOW);
  }
  else if (randomLed == LED_6)
  {
    digitalWrite(BLUE_PIN_2, HIGH);
    digitalWrite(RED_PIN_2, LOW);
    digitalWrite(GREEN_PIN_2, LOW);
    digitalWrite(BLUE_PIN_1, LOW);
    digitalWrite(RED_PIN_1, LOW);
    digitalWrite(GREEN_PIN_1, LOW);
  }
}