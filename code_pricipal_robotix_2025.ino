#include <Servo.h>

Servo Servo_elevateur;
Servo Servo_planche;
Servo Servo_aimant_centre;
Servo Servo_aimant_lateraux;

int btn_elevateur = 23;       // Broche 23 sur l'arduino mega
int btn_aimant_centre = 24;   // Broche 24 sur l'arduino mega
int btn_aimant_lateraux = 4;
int btn_planche = 5;

boolean val_btn_elevateur,
        val_btn_aimant_centre,
        val_btn_aimant_lateraux,
        val_btn_planche;

const int PWM2A = 11;     // Broche PWM du moteur M1
const int PWM2B = 3;      // Broche PWM du moteur M2
const int PWM0A = 6;      // Broche PWM du moteur M3
const int PWM0B = 5;      // Broche PWM du moteur M4
const int DIR_CLK = 4;    // Ligne d'horloge d'entrée de données
const int DIR_EN = 7;     // Broches d'activation du L293D
const int DATA = 8;       // Câble USB
const int DIR_LATCH = 12; // Horloge de verrouillage de la mémoire de sortie

const int Move_Forward = 39;      // Avancer
const int Move_Backward = 216;    // Reculer
const int Left_Move = 116;        // Translation vers la gauche
const int Right_Move = 139;       // Translation vers la droite
const int Right_Rotate = 149;     // Rotation à droite
const int Left_Rotate = 106;      // Rotation à gauche
const int Stop = 0;               // Arrêt
const int Upper_Left_Move = 36;   // Déplacement en haut à gauche
const int Upper_Right_Move = 3;   // Déplacement en haut à droite
const int Lower_Left_Move = 80;   // Déplacement en bas à gauche
const int Lower_Right_Move = 136; // Déplacement en bas à droite
const int Drift_Left = 20;        // Dérive vers la gauche
const int Drift_Right = 10;       // Dérive vers la droite

int Speed1 = 255; // Définir la vitesse par défaut entre 1 et 255
int Speed2 = 255; // Définir la vitesse par défaut entre 1 et 255
int Speed3 = 255; // Définir la vitesse par défaut entre 1 et 255
int Speed4 = 255; // Définir la vitesse par défaut entre 1 et 255

/******************JOYSTICK****************/
const int JOYSTICK_X_GAUCHE = A0;
const int JOYSTICK_Y_GAUCHE = A1;
const int JOYSTICK_X_DROIT = A2;
const int JOYSTICK_Y_DROIT = A3;
const int JOYSTICK_BUTTON_GAUCHE = 2;
const int JOYSTICK_BUTTON_DROIT = 3;

int X_GAUCHE = 0;
int Y_GAUCHE = 0;
int X_DROIT = 0;
int Y_DROIT = 0;

// Zone morte du joystick gauche
const int ZONE_MORTE = 150;
const int ZONE_MORT_CENTRE = 512;
const int VITESSE_MAX = 255;
// botons reset
int btn_RESET = 6;

void Motor(int Dir, int Speed1, int Speed2, int Speed3, int Speed4)
{
  analogWrite(PWM2A, Speed1); // Régulation de la vitesse du moteur par PWM
  analogWrite(PWM2B, Speed2); // Régulation de la vitesse du moteur par PWM
  analogWrite(PWM0A, Speed3); // Régulation de la vitesse du moteur par PWM
  analogWrite(PWM0B, Speed4); // Régulation de la vitesse du moteur par PWM

  digitalWrite(DIR_LATCH, LOW);           // DIR_LATCH passe au niveau bas et prépare l'écriture de la direction
  shiftOut(DATA, DIR_CLK, MSBFIRST, Dir); // Écriture de la valeur de direction Dir
  digitalWrite(DIR_LATCH, HIGH);          // DIR_LATCH passe au niveau haut et sort la direction
}


void setup() {
  Serial.begin(9600);
  // boutons
  pinMode(btn_elevateur, INPUT_PULLUP);
  pinMode(btn_aimant_centre, INPUT_PULLUP);
  pinMode(btn_aimant_lateraux, INPUT_PULLUP);
  pinMode(btn_planche, INPUT_PULLUP);

  // Joystick
  pinMode(DIR_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(DIR_EN, OUTPUT);
  pinMode(DIR_LATCH, OUTPUT);
  pinMode(PWM0B, OUTPUT);
  pinMode(PWM0A, OUTPUT);
  pinMode(PWM2A, OUTPUT);
  pinMode(PWM2B, OUTPUT);

  /******************JOYSTICK****************/
  pinMode(JOYSTICK_X_GAUCHE, INPUT);
  pinMode(JOYSTICK_Y_GAUCHE, INPUT);
  pinMode(JOYSTICK_X_DROIT, INPUT);
  pinMode(JOYSTICK_Y_DROIT, INPUT);
  pinMode(JOYSTICK_BUTTON_GAUCHE, INPUT);
  pinMode(JOYSTICK_BUTTON_DROIT, INPUT);

  // Servomoteurs
  Servo_elevateur.attach(53);         // Borhce 53 sur l'arduino mega
  Servo_planche.attach(52);           // Borhce 52 sur l'arduino mega
  Servo_aimant_centre.attach(51);     // Borhce 51 sur l'arduino mega
  Servo_aimant_lateraux.attach(50);
  delay(10);
  Servo_elevateur.write(90); // On arrête le servo
  Servo_planche.write(0);
  Servo_aimant_centre.write(0);
  Servo_aimant_lateraux.write(0);
  delay(500);
  
}

void loop() {
  // recupt valeurs boutons
  val_btn_elevateur = digitalRead(btn_elevateur);
  val_btn_aimant_centre = digitalRead(btn_aimant_centre);
  val_btn_aimant_lateraux = digitalRead(btn_aimant_lateraux);
  val_btn_planche = digitalRead(btn_planche);

  Serial.println(val_btn_elevateur);

  // Lecture des valeurs des joysticks
  X_GAUCHE = analogRead(JOYSTICK_X_GAUCHE);
  Y_GAUCHE = analogRead(JOYSTICK_Y_GAUCHE);
  X_DROIT = analogRead(JOYSTICK_X_DROIT);
  Y_DROIT = analogRead(JOYSTICK_Y_DROIT);

  // Calcul de la vitesse en fonction de la distance au centre
  int vitesse = 0;

  // Gestion des mouvements avec vitesse variable
  if (Y_GAUCHE < (ZONE_MORT_CENTRE - ZONE_MORTE)){
    // Avancer - Plus on pousse vers l'avant, plus la vitesse augmente
    vitesse = map(Y_GAUCHE, 0, ZONE_MORT_CENTRE - ZONE_MORTE, VITESSE_MAX, 0);
    Motor(Move_Forward, vitesse, vitesse, vitesse, vitesse);
  }
  else if (Y_GAUCHE > (ZONE_MORT_CENTRE + ZONE_MORTE)){
    // Reculer - Plus on tire vers l'arrière, plus la vitesse augmente
    vitesse = map(Y_GAUCHE, ZONE_MORT_CENTRE + ZONE_MORTE, 1023, 0, VITESSE_MAX);
    Motor(Move_Backward, vitesse, vitesse, vitesse, vitesse);
  }
  else if (X_GAUCHE < (ZONE_MORT_CENTRE - ZONE_MORTE)){
    // Translation droite - Plus on pousse à droite, plus la vitesse augmente
    vitesse = map(X_GAUCHE, 0, ZONE_MORT_CENTRE - ZONE_MORTE, VITESSE_MAX, 0);
    Motor(Right_Move, vitesse, vitesse, vitesse, vitesse);
  }
  else if (X_GAUCHE > (ZONE_MORT_CENTRE + ZONE_MORTE)){
        // Translation gauche - Plus on pousse à gauche, plus la vitesse augmente
    vitesse = map(X_GAUCHE, ZONE_MORT_CENTRE + ZONE_MORTE, 1023, 0, VITESSE_MAX);
    Motor(Left_Move, vitesse, vitesse, vitesse, vitesse);
  }
  // Ajout du contrôle des rotations avec le joystick droit
  else if (X_DROIT < (ZONE_MORT_CENTRE - ZONE_MORTE)){
    // Rotation droite - Plus on pousse à droite, plus la rotation est rapide
    vitesse = map(X_DROIT, 0, ZONE_MORT_CENTRE - ZONE_MORTE, VITESSE_MAX, 0);
    Motor(Right_Rotate, vitesse, vitesse, vitesse, vitesse);
  }
  else if (X_DROIT > (ZONE_MORT_CENTRE + ZONE_MORTE)){
    // Rotation gauche - Plus on pousse à gauche, plus la rotation est rapide
    vitesse = map(X_DROIT, ZONE_MORT_CENTRE + ZONE_MORTE, 1023, 0, VITESSE_MAX);
    Motor(Left_Rotate, vitesse, vitesse, vitesse, vitesse);
  }
  else{
    // Zone morte - Arrêt
    Motor(Stop, 0, 0, 0, 0);
  }

  // Affichage des valeurs sur le port série
  Serial.print("X_GAUCHE: ");
  Serial.print(X_GAUCHE);
  Serial.print(" | ");
  Serial.print(" Y_GAUCHE: ");
  Serial.print(Y_GAUCHE);
  Serial.print(" | ");
  Serial.print(" X_DROIT: ");
  Serial.print(X_DROIT);
  Serial.print(" | ");
  Serial.print(" Vitesse: ");
  Serial.println(vitesse);

  gestion_boutons();

  delay(10);
}

void gestion_boutons(){
  if (val_btn_elevateur == 0)
  {
    Serial.println("Elevation du rateau");
    Servo_elevateur.attach(53);
    Servo_elevateur.write(125);
    delay(100);
  }
  else{
    Servo_elevateur.detach();
    Servo_elevateur.write(90);
    delay(100);
  }

  if (val_btn_aimant_centre == 0){
    Serial.println("Detachement aimant centraux");
    Servo_aimant_centre.write(90);
    delay(100);
  }
  else{
    Servo_aimant_centre.write(0);
    delay(100);
  }

  if (val_btn_aimant_lateraux != 0){
    Servo_aimant_lateraux.write(90);
    delay(100);
  }
  else
  {
    Servo_aimant_lateraux.write(0);
    delay(100);
  }

  if (val_btn_planche != 0)
  {
    Servo_planche.write(90);
    delay(100);
  }
  else
  {
    Servo_planche.write(0);
    delay(100);
  }
}
