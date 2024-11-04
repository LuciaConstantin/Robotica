#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define RED_PIN 6
#define GREEN_PIN 5
#define BLUE_PIN 4
#define START_STOP_BUTTON 3
#define MODE_BUTTON 2

// scorul obtinut dupa fiecare runda
volatile int score;
// variabila care memoreaza starea butonului de mod easy-0, medium-1, hard-2
volatile int mode = 0;
// starea in care se jocul
bool roundState = false;

/*variabila care memoreaza de cate ori se intra pe intreruperea timerului 2,
folosita pentru frecventa de aparitie a cuvintelor*/
volatile long interruptCount = 0;
volatile int easyCount = 4000;
volatile int mediumCount = 2000;
volatile int hardCount = 1000;
volatile int timeRoundCount = 33;

/* variabilele si constantele folosite pentru logica de generarea a cuvintelor si
de comparare a cuvantului generat de joc cu cel introdus de catre utilizator*/
const char *dictionary[] = {
    "pisica", "masina", "copac", "rac", "carte", "semafor", "ploaie", "gradina",
    "manusi", "robotica", "pinguin", "foisor", "dragon", "lalele", "breadboard", "cer",
    "imagine", "bicicleta", "calculator", "tema", "coincidenta", "intrerupere", "timer", "caine", "entuziasm",
    "penar", "enumeratie", "saptamana", "ghiocel", "cultura", "accent", "vaza", "dulap", "lampa"};

volatile int wordsNumer = sizeof(dictionary) / sizeof(dictionary[0]);
volatile int randomWord;
volatile char letter; // litera transmisa de catre jucator in timpul rundei
volatile int wordLength = 0;
volatile int letterNumber;
char userWord[100];
char gameWord[100];
volatile int newWord = false; /*varaibila folosita pentru a ne asigura ca noul cuvant aparut in joc
se afiseaza o singura data nu de mai multe ori, pentru ca ne aflam in loop()*/


/* Partea de definire a variabilelor si constantelor folosite pentru logica de debounce a butoanelor*/
volatile int previousStateRound = HIGH;
volatile unsigned long previousDebounceRound = 0;
const int delayRound = 200;

volatile int previuousStateMode = 3;
volatile unsigned long previousDebounceMode = 0;
const int delayMode = 200;

/*functia apelata pentru intreruperea la apasarea butonului de Start/Stop,
aceasta contine si logica de debouncing a butonului
compara daca timpul dintre intreruperea actuala si cel de la intreruperea anterioara sunt mai mari decat un anumit interval
+ schimbarea starii in care se afla jocul */
void debounceRound()
{
  if (millis() - previousDebounceRound > delayRound)
  {
    previousDebounceRound = millis();
    roundState = !roundState;
  }
}

/* functia apelata in cazul intreruperii la apasarea butonului de mod, contine
logica de debounce a butonului si ciclarea butonului de mod intre easy-medium-hard
si se executa doar in cazul in care jocul se afla in repaus*/
volatile int easy = 0;
volatile int medium = 1;
volatile int hard = 2;
void debounceMode()
{
  if (!roundState)
  {
    if (millis() - previousDebounceMode > delayMode)
    {
      previousDebounceMode = millis();
      mode++;
      if (mode > hard)
        mode = easy;
    }
  }
}

/*variabilele si constantele folosite pentru partea de clipire a led-ului*/
const unsigned long blinkInterval = 1000;
unsigned long previousBlinkTime = 0;
int ledState = LOW;
unsigned long startBlinkTime = 0;
volatile int counter; // de cate ori trebuie sa clipeasca led-ul
volatile unsigned long currentTime;
volatile bool ledBlinkState = true;
volatile int stateNumber = 4;

/* functia ce se ocupa de logica de clipire a led-ului, schimba starea led-ului la un anumit interval de timp si se realizeaza
de un numar exact de ori, stabilit de catre counter pentru a respecta conditia de a clipi timp de 3 secunde,
la finalul partii de clipire a led-ului acesta va ramane verde, iar in functie de modul in care se afla runda va seta
variabila interruptCount pentru a se executa codul de generare a cuvintelor imediat dupa terminarea partii de clipire a led-ului*/
void blinkLED()
{
  ledBlinkState = true;
  currentTime = millis();
  startBlinkTime = millis();
  previousBlinkTime = 0;
  ledState = HIGH;
  counter = stateNumber;
  while (counter && roundState == true)
  {
    if (millis() - previousBlinkTime > blinkInterval)
    {
      previousBlinkTime = millis();
      ledState = !ledState;

      if (ledState == LOW)
      {
        counter--;
      }
      else
        Serial.println(counter);
      digitalWrite(GREEN_PIN, ledState);
      digitalWrite(RED_PIN, ledState);
      digitalWrite(BLUE_PIN, ledState);
    }
  }

  if (roundState)
  {
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }

  switch (mode)
  {
  case 0:
    interruptCount = easyCount;
    break;
  case 1:
    interruptCount = mediumCount;
    break;
  default:
    interruptCount = hardCount;
  }
}

/* variabila folosita pentru incrementarea intreruperilor pentru timer 1*/
volatile int interruptRoundCounter = 0;

/*intreruperea din timer 1, daca runda este activa se va incrementa counterul,
daca se afla in repaus se reseteaza counterele la 0
se va apela la fiecare 1s din cauza frecventei de 1Hz */
ISR(TIMER1_COMPA_vect)
{
  if (roundState)
    interruptRoundCounter++;
  if (!roundState)
  {
    interruptCount = 0;
    interruptRoundCounter = 0;
  }
}

/* intializare timer 1
 timer 1 cu frecventa 1Hz, folosit pentru a masura timpul alocat rundei (30 s)*/
void initTimer1()
{
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 15624;                       // valoarea cu care va fi comparat TCNT1
  TCCR1B |= (1 << WGM12);              // CTC
  TCCR1B |= (1 << CS12) | (1 << CS10); // prescaler 1024
  TIMSK1 |= (1 << OCIE1A);             // activarea intreruperii

  interrupts();
}

volatile bool flag = false; // setează un flag la fiecare întrerupere, pentru a stii daca s-a activat sau nu timerul
/*intreruperea lui timer 2, va fi apelata la fiecare 2 ms din cauza frecventei de 500 Hz,
folosita pentru a incrementa counter-ul la fiecare apel*/
ISR(TIMER2_COMPA_vect)
{
  flag = true;
  interruptCount++;
}

/*Timer 2 este folosit pentru a gestiona frecventa de aparitie a cuvintelor in functie de mod*/
void initTimer2()
{
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT2 = 0;

  TCCR2A = (1 << WGM21);               // CTC
  TCCR2B |= (1 << CS22) | (1 << CS21); // prescaler 256
  OCR2A = 124;                         // valoarea cu care se va compara TCNT2
  TIMSK2 |= (1 << OCIE2A);             // activeaza interuperea

  interrupts();
}

void setup()
{
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(START_STOP_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(START_STOP_BUTTON), debounceRound, FALLING);
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), debounceMode, FALLING);
  randomSeed(analogRead(0)); // seed-ul setat pe un pin care nu este folosit
  initTimer2();
  initTimer1();
}

void loop()
{
  /*apasare buton Start/Stop*/
  /*daca starea jocului devinde din repaus in activ, scorul va trece in 0 si se va apela o singura data functia de clipire a led-ului
  in cazul in care starea jocului devine din activa in repaus, se va afisa scorul rundei si led-ul va ramane de culoarea alba si nu va clipi*/
  if (roundState != previousStateRound)
  {
    if (roundState)
    {
      score = 0;
      blinkLED();
      ledBlinkState = false;
    }
    else
    {
      Serial.println("scorul este");
      Serial.println(score);
      ledState = HIGH;
      digitalWrite(GREEN_PIN, ledState);
      digitalWrite(RED_PIN, ledState);
      digitalWrite(BLUE_PIN, ledState);
    }
    previousStateRound = roundState;
  }

  /*afisarea modului in care se va afla urmatoarea runda, afisarea se va face doar cand jocul este in repaus*/
  if (!roundState && mode != previuousStateMode)
  {
    ledState = HIGH;
    digitalWrite(GREEN_PIN, ledState);
    digitalWrite(RED_PIN, ledState);
    digitalWrite(BLUE_PIN, ledState);
    
    previuousStateMode = mode;
    switch (mode)
    {
    case 0:
      Serial.println("Easy mode on!");
      break;
    case 1:
      Serial.println("Medium mode on!");
      break;
    case 2:
      Serial.println("Hard mode on!");
      break;
    }
  }

  /*daca runda este activa, dar counter-ul ce memoreaza timpul scurs de la inceperea rundei ajunge la valoarea de final
  jocul se termina si counterele folosite pentru memeorarea timpului rundei si cel pentru frecventa cuvintelor se initializeaza din nou de la 0*/
  if (roundState)
  {
    if (interruptRoundCounter == timeRoundCount)
    {
      roundState = false;
      interruptRoundCounter = 0;
      interruptCount = 0;
    }
  }

  /*daca runda este activa si led-ul a terminat din clipit, si s-a inceput si numaratoarea
  timpului de la inceputul rundei, atunci se realizeza logica jocului*/
  if (flag && roundState == true && !ledBlinkState)
  {
    flag = false;
    /*daca counter-ul pentru frecventa aparitiei cuvintelor corespunde si cu modul in care se afla jocul atunci o sa generam un cuvant random*/
    if ((interruptCount >= easyCount && mode == easy) || (interruptCount >= mediumCount && mode == medium) || (interruptCount >= hardCount && mode == hard))
    {
      randomWord = random(wordsNumer);
      newWord = false;
      interruptCount = 0;
      //strcpy(gameWord, dictionary[randomWord]);
      gameWord[0] = '\0';
      userWord[0] = '\0';
      letterNumber = 0;
      wordLength = strlen(dictionary[randomWord]);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      letter= '\0';
    }
    else
    {
      /*pentru ca ne aflam intr-un loop(), am folosit newWord pentru a se afisa cuvantul generat o singura data*/
      if (newWord == false)
      {
        Serial.println(dictionary[randomWord]);
        newWord = true;
      }

      if (Serial.available() > 0)
      {
        letter = Serial.read();
        if (letterNumber == 0)
        {
          digitalWrite(GREEN_PIN, HIGH);
          digitalWrite(RED_PIN, LOW);
        }

        /* in cazul in care utilizatorul greseste folosind tasta backspace se va sterge din litere
        altfel se va aduauga noua litera citita la cuvantul construit si se va adauga si o noua litera la cuvantul de baza
        cuvantul scris de utilizator si cel generat se vor construi litera cu litera*/
        if (letter == '\b' && letterNumber >= 0)
        {
          if (letterNumber > 0)
          {
            letterNumber--;
            userWord[letterNumber] = '\0';
            gameWord[letterNumber] = '\0';
            // digitalWrite(RED_PIN, LOW);
          }
          if (letterNumber == 0)
          {
            digitalWrite(GREEN_PIN, HIGH);
            digitalWrite(RED_PIN, LOW);
          }
        }
        else if(letterNumber <= wordLength)
        {
          userWord[letterNumber] = letter;
          gameWord[letterNumber] = dictionary[randomWord][letterNumber];
          letterNumber++;
          userWord[letterNumber] = '\0';
          gameWord[letterNumber] = '\0';
        }

        /*utilizatorul va scrie cuvantul, iar daca litera din cuvantul scris de utilizator corespunde cu 
        cuvantului generat led-ul ramane verde, altfel devine rosu*/
        if (letterNumber <= wordLength && letterNumber != 0 && letterNumber > 0)
        {
          if (strcmp(userWord, gameWord) == 0 )
          {
            digitalWrite(GREEN_PIN, HIGH);
            digitalWrite(RED_PIN, LOW);
          }
          else if (userWord[letterNumber - 1] != gameWord[letterNumber - 1])
          {
            digitalWrite(GREEN_PIN, LOW);
            digitalWrite(RED_PIN, HIGH);
          }
        }
        /* daca cuvantul scris de utilizator corespunde cu cel generat, atunci se creste scorul si se trece imediat la urmatorul cuvant*/
        if ((strcmp(userWord, gameWord) == 0) && letterNumber == wordLength)
        {
          score++;
          digitalWrite(GREEN_PIN, HIGH);
          digitalWrite(RED_PIN, LOW);

          if (mode == 0)
          {
            interruptCount = easyCount;
          }
          else if (mode == 1)
          {
            interruptCount = mediumCount;
          }
          else
          {
            interruptCount = hardCount;
          }
        }
      }
    }
  }
}
