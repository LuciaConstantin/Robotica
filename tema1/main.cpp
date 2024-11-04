#include <Arduino.h>

//definire variabile si constante folosite in program
/* definirea pinilor folositi pentru cele 5 led-uri si 2 butoane*/

int loaderLed[4]={10,9,8,7}; //pinii ce reprezinta led-urile de incarcare
#define RED_PIN 6
#define GREEN_PIN 5
#define START_BUTTON 3
#define STOP_BUTTON 2

/* definirea variabilelor folosite pentru debouncing-ul celor doua butoane Start si Stop*/

volatile int stareButonStart;   //starea actuala a butonului      
volatile int stareAnterioaraStart = HIGH; //starea anterioara a butonului
volatile unsigned long timpAnteriorDebounceStart = 0; //ultima data cand starea butonului a fost modificata
const int intarziereDebounceStart = 50; //timpul de debounce

volatile int stareButonStop;            
volatile int stareAnterioaraStop = HIGH;
volatile unsigned long timpAnteriorDebounceStop = 0;
const int intarziereDebounceStop = 1000;

/*definirea variabilelor si constantelor folosite pentru logica led-urilor */

const unsigned long timpClipire= 3000; //intervalul fix de timp in care trebuie sa se realizeze clipirea
const int nrLed= 4; //numarul de led-uri din loader-ul de incarcare
volatile int k = 0;  //variabila prin care vom itera prin vectorul de led-uri
const unsigned long intervalClipire= 500; //la ce interval se realizeaza o modificare a starii unui led care clipeste
unsigned long timpAnteriorClipire = 0;
volatile int stareLED= LOW;
unsigned long inceputTimpClipire = 0; 
const int nrClipire= 7; //constanta ce reprezinta nr de stari low, high ale led-urilor pentru a se aprinde si stinge de 3 ori

/*variabile bool folosite pentru gestionarea incarcarii si finalului acesteia*/
bool stopIncarcare = false;
bool incarcareActiva = false;


/*configurarea pini-lor pentru led-uri si butoane*/

void setup()
{
  Serial.begin(9600);
  int i;
  for(i=0; i<nrLed; i++)
  {
    pinMode(loaderLed[i], OUTPUT);
  }
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(STOP_BUTTON, INPUT_PULLUP);

}


/*functie folosite pentru actiunea de clipire simultana a led-urilor de la finalul incarcarii
sau de le oprirea fortata a incarcarii*/

void blinkFinal(){
  int l=0;
  stareLED= LOW;

  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);

  /*pentru a realiza 3 clipiri led-urile vor trece prin nrClipire(7) stari, din aceasta cauza am folosit un while in 
  care am introdus conditii de toggle intre starile de low si high ale led-urilor*/
  while(l< nrClipire)
  {
    delay(intervalClipire);
    for(int j=0; j<4; j++)
      digitalWrite(loaderLed[j], stareLED);

    if(stareLED == LOW)
        stareLED= HIGH;
    else
        stareLED= LOW;
    l++;
      
  } 
}




void loop()
{
  /*gestionarea logicii apasarii butonului de start*/
  //aflam starea butonului
  int citireStart = digitalRead(START_BUTTON);
  
  /*daca starea anterioara este diferita de starea actuala, 
  atunci o sa memoram momentul in care am sesizat apasarea butonului si resetam timerul de debouncing*/
  if (citireStart != stareAnterioaraStart) {
    timpAnteriorDebounceStart = millis();
  }

  /*daca timpul scurs de la ultima schimbare a starii depaseste intervalul de debounce, 
  putem considera ca avem un semnal stabil si ca butonul a fost apasat cu siguranta
  iar starea statiei devine activa, sau ramane activa daca inca nu s-a terminat procesul de incarcare*/
  if ((millis() - timpAnteriorDebounceStart) > intarziereDebounceStart) {
    if(citireStart != stareButonStart){
      stareButonStart= citireStart;
      if(stareButonStart == LOW)
      {
        incarcareActiva= true;
      }
      
    }
    
  }
  stareAnterioaraStart = citireStart;
  

  /*gestionarea led-ului RGB atunci cand procesul de incarcare nu mai este activ, LED-ul trebuie sa fie verde*/
  if(!incarcareActiva)
  {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
  }
 
  /*gestionarea statiei de incarcare atunci cand in statie are loc procseul de incarcare*/
  if (incarcareActiva) {
  
    /*gestionare logicii apasarii butonului de stop incarcare, aproape identica ca cea a butonului de start*/
    int citireStop = digitalRead(STOP_BUTTON);
  
    if (citireStop != stareAnterioaraStop) {
      timpAnteriorDebounceStop = millis();
    }
  
    if ((millis() - timpAnteriorDebounceStop) >= intarziereDebounceStop) {
  
      if(citireStop != stareButonStop){
        stareButonStop= citireStop;
        if(stareButonStop == LOW)
        {
          //daca butonul de stop este apasat, semnalizam oprirea incarcarii 
          stopIncarcare= true;
          k= nrLed;
        }
        
      }
      
    }

    stareAnterioaraStop = citireStop;
    
    /*gestionarea procesului de clipire atunci cand nu a fost apasat butonul de stop*/
    if(stopIncarcare == false){
      
      //mentinerea unei culori corecte al led-ului RGB in timpul incarcarii
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);

      //partea de clipire a led-urilor
      /*daca este prima data cand un led incepe sa clipeasca, memoram acest moment*/
      unsigned long timpCurent = millis();
      if (inceputTimpClipire == 0) {
        inceputTimpClipire = timpCurent; 
        timpAnteriorClipire= timpCurent;
        digitalWrite(loaderLed[k], HIGH);
      }

      /*verificam daca a trecut suficient timp de la ultima schimbare a ledului si daca am ajuns la finalul loader-ului
      in cazul in care ambele conditii cunt adevarate schimbam starea led-ului pentru a avea fenomenul de clipire*/
      if (timpCurent - timpAnteriorClipire >= intervalClipire && k < nrLed) {
        timpAnteriorClipire = timpCurent;  
        if (stareLED == LOW) 
          stareLED = HIGH;
        else 
          stareLED = LOW;
        
        digitalWrite(loaderLed[k], stareLED);
        
      }

      /*daca a trecut timpul fixat de 3s in care trebuie sa clipeasca led-ul il lasam aprins si trecem la urmatorul, 
      ce va avea desigur timpul de inceput de clipire egal cu 0 */
      else if (timpCurent - inceputTimpClipire >= timpClipire) { 
          digitalWrite(loaderLed[k], HIGH);
          k++; 
          inceputTimpClipire=0;
      }

    }
    /*resetarea tuturor variabilelor in cazul finalului incarcarii si aprinderea corecta a led-ului verde*/
    if (k == nrLed) {
      blinkFinal(); 
      incarcareActiva = false; 
      k=0;
      inceputTimpClipire= 0;
      stopIncarcare= false;
      stareButonStop= HIGH;
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);

    }
  }
}
    


