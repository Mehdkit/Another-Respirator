/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
 
 This example code is in the public domain.
 */
#include <LiquidCrystal.h>

#define ATTENTE 0
#define INSPIRATION 1
#define EXPIRATION 2

#define PIN_PRESS 1
#define PIN_BPM 3
#define PIN_IE 4
#define PIN_PRESS_MAX 2
#define PIN_PENTE 0
#define PIN_TRIG 5

#define PIN_RUPTEUR 1
#define PIN_INSPI_COMMANDE 2
#define PIN_MARCHE_PAUSE 3

#define PIN_COMMANDE_MOTEUR 2
#define PIN_ALARM_BUZZER 3

#define PIN_RESET_BUTTON 1

#define PIN_LCD_E 2
#define PIN_LCD_RS 1
#define PIN_LCD_DB4 2
#define PIN_LCD_DB5 1
#define PIN_LCD_DB6 2
#define PIN_LCD_DB7 3

#define BPM_MAX 30
#define PRESS_LIMIT 200
#define TRIG_MIN 1

int pressure_value;

float bpm_setting;
float IE_ratio_setting;

int trigger_setting;
int pente_insp_setting;

int etat = 0;
float alpha = 0;

boolean rupteur_cycle = false;
boolean end_cycle = false;
boolean marche = false;
boolean too_much_pressure = false;

LiquidCrystal Screen(PIN_LCD_E, PIN_LCD_RS, PIN_LCD_DB4, PIN_LCD_DB5, PIN_LCD_DB6, PIN_LCD_DB7);

// the setup routine runs once when you press reset:
void setup() {
  //initialisation de l'écran LCD
  Screen.begin(16,4);
  Screen.clear();
  //initialisation des entrées/sorties
  pinMode(PIN_RUPTEUR,INPUT);
  pinMode(PIN_INSPI_COMMANDE,INPUT);
  pinMode(PIN_MARCHE_PAUSE,INPUT);
  pinMode(PIN_COMMANDE_MOTEUR,OUTPUT);
  pinMode(PIN_ALARM_BUZZER,OUTPUT);
  pinMode(PIN_RESET_BUTTON,INPUT);

  //initialisation des interuptions
  attachInterrupt(digitalPinToInterrupt(PIN_INSPI_COMMANDE), interruptCommande, RISING);

  //initialisation des timers
}

//--------------------INTERRUPTION--------------------------
void interruptT1(){
  if(etat == ATTENTE && rupteur_cycle == false){
    etat = EXPIRATION;
  }
}

void interruptT2(){
  if(etat == ATTENTE && rupteur_cycle == true){
    etat = INSPIRATION;
  }
}

void interruptCommande(){
  if(etat == ATTENTE && rupteur_cycle == true){
    etat = INSPIRATION;
  }
}

// the loop routine runs over and over again forever:
void loop() {
  //-------------LECTURE DES ENTREE-----------------
  
  //Leture des contrôles de pression 
  too_much_pressure = analogRead(PIN_PRESS_MAX);
  pressure_value = analogRead(PIN_PRESS);

  //lecture des réglages
  bpm_setting = analogRead(PIN_BPM);
  IE_ratio_setting = analogRead(PIN_IE);
  trigger_setting = analogRead(PIN_TRIG);
  pente_insp_setting = analogRead(PIN_PENTE);

  //lecture de l'etat du rupteur
  rupteur_cycle = digitalRead(PIN_RUPTEUR);
  marche = digitalRead(PIN_MARCHE_PAUSE);
  //-------------GESTION DES ETATS-----------------
  
  if(marche){
    
    switch(etat){

    case ATTENTE:
      //Valeur de alpha
      alpha = 0;
      if(trigger_setting > TRIG_MIN && pressure_value < trigger_setting && rupteur_cycle == true){
        etat = INSPIRATION;
      }
    break;
    
    case INSPIRATION:
      //Valeur de alpha
      alpha = pente_insp_setting;
      //Condition de sortie
       if(rupteur_cycle == false){
        //on lance le timer de plateau inspiratoire
        
        //on passe à l'état ATTENTE
         etat = ATTENTE;
       }
    break;
    
    case EXPIRATION:
      //Valeur de alpha
      alpha = pente_insp_setting;
      //Condition de sortie
      if(rupteur_cycle == true){
        //On lance le timer de plateau expiratoire
        
        //On se place en etat d'attente
        etat = ATTENTE;
      }
      break;
    }
    //Envoi au moteur de sa commande actuelle
    analogWrite(PIN_COMMANDE_MOTEUR,alpha);
  }else{
    alpha = 0;
  }

  //-------------AFFICHAGE-----------------
  Screen.setCursor(0,0);
  Screen.print("BPM : ");
  Screen.print(bpm_setting);
  Screen.setCursor(9,0);
  Screen.print("| IE : ");
  Screen.print(IE_ratio_setting);
  Screen.setCursor(0,1);
  Screen.print("TRIG : ");
  Screen.print(trigger_setting);
  Screen.setCursor(9,1);
  Screen.print("| PENTE : ");
  Screen.print(pente_insp_setting);
  Screen.setCursor(0,2);
  Screen.print("----------------");
  Screen.setCursor(0,3);
  Screen.print("PRESSURE : ");
  Screen.print(pressure_value);

  delay(1);        // delay in between reads for stability
}
