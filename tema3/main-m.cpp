#include <Arduino.h>
#include <SPI.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>
#include "ServoTimer2.h"

// which button has been pressed
#define BTN_1 1
#define BTN_2 2
#define BTN_3 3
#define BTN_START 4
#define BTN_5 5
#define BTN_6 6
#define BTN_7 7
#define NO_BTN -1

// LED_1  for button 1 , LED_2 for button 2 ...
#define LED_1 1
#define LED_2 2
#define LED_3 3
#define LED_4 4
#define LED_5 5
#define LED_6 6

int highScore;
String highScoreName;


volatile bool matched = false; // if the button corresponding to the RGB led was pressed

// LCD variables
const int rs = 3, en = 8, d4 = 7, d5 = 6, d6 = 9, d7 = 4; // pins for LCD control
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                // creating the lcd object using the defined pins
String Player1Name, Player2Name;
int playerNumber = 1;
int playerBegin = 1;
int afterGameTime = 2000;
int finishTime = 0;

// starea jocului
volatile bool gameState = false;
volatile bool finalScoreGameState = false;

// SPI setting and variables
SPISettings settingsA(2000000, MSBFIRST, SPI_MODE0);
int confirmedButton;
byte Mastersend;
#define interruptPin 2

// Servo settings
ServoTimer2 myservo;
int servoAngle = 750; // 0 degrees on servo
#define anglesPerSecond 25
#define servoPin 5

int lcdStart = true;

// player
#define player1 1
#define player2 2
volatile int activePlayer = player2;

// time
const int gameTime = 60;
const int roundTime = 15;
volatile bool oneTime = false; // every new round the player is changed only once

// score
volatile int scorePlayer1 = 0;
volatile int scorePlayer2 = 0;
volatile int maximumPoints = 100;
volatile int minimumPoints = 1;
unsigned long activeLed = 0;
unsigned long reactionTime = 0;
const int maximumResponseTime = 10000;

// random led generator variables
uint8_t randomLed;
const int optionsLEDPlayer1 = 4;
const int optionsLEDPlayer2 = 7;
const int firstLEDPlayer1 = 1;
const int firstLEDPlayer2 = 4;

// timer
volatile int interruptRoundCounter = 0;
/*the function checks the game state and updates the servo angle by 3 degrees per second (25 units per interrupt),
and resets the game when the specified time limit is reached. */
ISR(TIMER1_COMPA_vect)
{
  if(gameState == true && interruptRoundCounter == gameTime)
    finishTime = millis();
  if (gameState == true && interruptRoundCounter >= gameTime)
  {
    interruptRoundCounter = 0;
    gameState = false;
  }
  if (gameState)
  {
    interruptRoundCounter++;
    servoAngle = servoAngle + anglesPerSecond;
  }

  if (!gameState)
  {
    interruptRoundCounter = 0;
  }
}

/* timer initialization 1
frequency 1Hz, used to measure the time allocated to the game*/
void initTimer1()
{
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 15624;                       // value to be compared to TCNT1
  TCCR1B |= (1 << WGM12);              // CTC
  TCCR1B |= (1 << CS12) | (1 << CS10); // prescaler 1024
  TIMSK1 |= (1 << OCIE1A);             // interrupt

  interrupts();
}

// function that receives the button from the slave
void spiTransfer()
{
  confirmedButton = SPI.transfer(Mastersend);
  Serial.println(confirmedButton);
}

void setup()
{
  interrupts();
  Serial.begin(9600);
  SPI.begin();                         // Begins the SPI commnuication
  SPI.setClockDivider(SPI_CLOCK_DIV8); // Sets clock for SPI communication at 8 (16/8=2Mhz)
  digitalWrite(SS, HIGH);
  initTimer1();
  SPCR |= _BV(SPIE);
  pinMode(interruptPin, INPUT_PULLUP);                            // Interrupt pin
  attachInterrupt(digitalPinToInterrupt(2), spiTransfer, CHANGE); // The slave makes a pin change to indicate to the master that it can request data on the SPI and read which button was pressed
  lcd.begin(16, 2);                                               // display init
  myservo.attach(servoPin);
  myservo.write(750);
  randomSeed(analogRead(1));
  Serial.println("Introduceti numele primului jucator\n");
}

void loop()
{
  digitalWrite(SS, LOW);

  // at the beginning of the game we read the names of the 2 players using Serial
  while (playerBegin == true)
  {
    


    if (playerNumber == 1)
    {
      while (Serial.available() > 0)
      {
        Player1Name = Serial.readString();
      }
      if (Player1Name != "")
      {
        playerNumber++;
        Serial.println("Introduceti numele celui de-al doilea jucator\n");
      }
    }
    else if (playerNumber == 2)
    {

      while (Serial.available() > 0)
      {
        Player2Name = Serial.readString();
      }
      if (Player2Name != "")
      {
        playerNumber++;
      }
    }
    else
    {
      playerBegin = false;
      Serial.println("Start Joc\n");
      lcdStart = true;
    }
  }

  if (lcdStart == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);

    lcd.print("Start joc");
    if(highScore !=  0){
      lcd.setCursor(0, 1);
      lcd.print("HI:");
      lcd.print(highScoreName);
      lcd.print("-");
      lcd.print(highScore);
      lcd.print("pct");
    }
    
      lcdStart = false;

  }

  /*the game start after the start button was pressed and if the game is not active
  the game varaibles are reseted
  */
  if (confirmedButton == BTN_START && gameState == false)
  {
    gameState = true;
    matched = true;
    activePlayer = player2;
    interruptRoundCounter = 0;
    oneTime = false;
    scorePlayer1 = 0;
    scorePlayer2 = 0;
    activeLed = 0;
    finalScoreGameState = true;
    confirmedButton = NO_BTN;
    lcd.clear();
  }

  // changing the player at the end of each round only once using oneTime
  if (gameState == true)
  {
    // rotate the servo with the angle incremented in the timer
    myservo.write(servoAngle);

    if (interruptRoundCounter % roundTime == 0 && oneTime == false)
    {
      if (oneTime == false)
      {
        if (activePlayer == player2)
          activePlayer = player1;
        else
          activePlayer = player2;
        oneTime = true;
        matched = true;
      }
    }

    if (interruptRoundCounter % roundTime != 0)
      oneTime = false;
  }

  // game logic
  // if player 1 is the active player
  if (gameState == true && activePlayer == player1)
  {
    // if the player pressed the correct button we transmit to the slave a random led
    if (matched == true)
    {
      randomLed = random(1, 4);
      Serial.println(randomLed);
      activeLed = millis();

      SPI.beginTransaction(settingsA);
      SPI.transfer(randomLed);
      SPI.endTransaction();
    }

    matched = false;
    // check if the button corresponding to the lit RGB LED was pressed
    if ((confirmedButton == BTN_1 && randomLed == LED_1) || (confirmedButton == BTN_2 && randomLed == LED_2) || (confirmedButton == BTN_3 && randomLed == LED_3))
    {
      matched = true;
      // score based on player reaction time
      reactionTime = millis() - activeLed;
      if (reactionTime < maximumResponseTime)
      {
        int scoreToAdd = maximumPoints - (reactionTime * maximumPoints / maximumResponseTime);
        scorePlayer1 += scoreToAdd;
      }
      else
      {
        scorePlayer1 += minimumPoints;
      }

      // update the player score
      lcd.setCursor(0, 0);
      lcd.print(Player1Name);
      lcd.print(":");
      lcd.print(scorePlayer1);
      lcd.print("pct");
    }
  }

  // player 2
  if (gameState == true && activePlayer == player2)
  {

    if (matched == true)
    {
      activeLed = millis();
      randomLed = random(4, 7);
      Serial.println(randomLed);

      SPI.beginTransaction(settingsA);
      SPI.transfer(randomLed);
      SPI.endTransaction();
    }

    matched = false;
    if ((confirmedButton == BTN_5 && randomLed == LED_4) || (confirmedButton == BTN_6 && randomLed == LED_5) || (confirmedButton == BTN_7 && randomLed == LED_6))
    {
      matched = true;
      reactionTime = millis() - activeLed;
      if (reactionTime < maximumResponseTime)
      {
        int scoreToAdd = maximumPoints - (reactionTime * maximumPoints / maximumResponseTime);
        scorePlayer2 += scoreToAdd;
      }
      else
      {
        scorePlayer2 += minimumPoints;
      }
      lcd.setCursor(0, 1);
      lcd.print(Player2Name);
      lcd.print(":");

      lcd.print(scorePlayer2);
      lcd.print("pct");
    }
  }
  // game is over
  if (!gameState)
  {
    if (finalScoreGameState == true)
    {
      finalScoreGameState = false;
      // reset player name for the next game
      playerBegin = true;
      // servo back to 0 degrees
      myservo.write(750);

      if(scorePlayer1 > highScore){
        highScore = scorePlayer1;
        highScoreName = Player1Name;
      }

      if(scorePlayer2 >highScore){
        highScore = scorePlayer2;
        highScoreName = Player2Name;
      }

    
    }
  }
}