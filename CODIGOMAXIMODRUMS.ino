#include <Arduino.h>

#include <Preferences.h>



// GPIOs dos Piezos (O Chimbal está no GPIO 12, que é o último do array - índice 11)

const int padPins[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}; 



// Pino do botão do Pedal de Chimbal (GPIO 13)

const int pedalPin = 13;



int midiNotes[12] = {36, 38, 48, 45, 43, 41, 49, 57, 51, 55, 39, 46};

int thresholds[12];

int gains[12];

int retrigger[12];

int curve[12];



bool padActive[12] = {false};

unsigned long lastHitTime[12] = {0};



// Estado do Pedal de Chimbal (GPIO 13)

bool pedalPressed = false;

bool lastPedalState = false;

unsigned long lastPedalDebounce = 0;



int openHHNote = 46;   // Chimbal Aberto

int closedHHNote = 42; // Chimbal Fechado

int footHHNote = 44;   // Pisada (Foot Close)



int hhPadIndex = 11;   // Índice 11 = GPIO 12 (Piezo do Chimbal)



Preferences prefs;



void loadSettings() {

  prefs.begin("drums", true); // Modo leitura

  for(int i = 0; i < 12; i++) {

    char k[6]; 

    sprintf(k, "t%d", i); thresholds[i] = prefs.getInt(k, 450);

    sprintf(k, "g%d", i); gains[i]      = prefs.getInt(k, 5);

    sprintf(k, "n%d", i); midiNotes[i]  = prefs.getInt(k, midiNotes[i]);

    sprintf(k, "r%d", i); retrigger[i]  = prefs.getInt(k, 30);

    sprintf(k, "c%d", i); curve[i]      = prefs.getInt(k, 0);

  }

  openHHNote  = prefs.getInt("hho", 46);

  closedHHNote = prefs.getInt("hhc", 42);

  footHHNote   = prefs.getInt("hhf", 44);

  prefs.end();

}



int applyCurve(int rawVel, int curveType) {

  float norm = rawVel / 127.0;

  if (curveType == 1) norm = log10(1 + 9 * norm);

  else if (curveType == 2) norm = pow(norm, 2);

  int finalVel = norm * 127;

  return constrain(finalVel, 1, 127);

}



void setup() {

  Serial.begin(115200);

  pinMode(pedalPin, INPUT_PULLUP);

  loadSettings();

}



void loop() {

  // 1. Leitura do Pedal de Chimbal (GPIO 13)

  bool currentPedalState = (digitalRead(pedalPin) == LOW); // LOW = Pressionado



  if (currentPedalState != lastPedalState) {

    if (millis() - lastPedalDebounce > 20) {

      pedalPressed = currentPedalState;

      

      // Quando pisa no pedal (Gera som de Foot Close)

      if (pedalPressed) {

        Serial.print("HIT:");

        Serial.print(hhPadIndex);

        Serial.print(":100:");

        Serial.println(footHHNote);

      }



      lastPedalState = currentPedalState;

      lastPedalDebounce = millis();

    }

  }



  // 2. Processamento dos Comandos vindos do Console HTML

  if (Serial.available() > 0) {

    String cmd = Serial.readStringUntil('\n');

    cmd.trim();

    

    if (cmd.startsWith("SET:")) {

      int p1 = cmd.indexOf(':', 4);

      int p2 = cmd.indexOf(':', p1 + 1);

      

      if (p1 != -1 && p2 != -1) {

        int pad = cmd.substring(4, p1).toInt();

        String type = cmd.substring(p1 + 1, p2);

        int val = cmd.substring(p2 + 1).toInt();

        

        prefs.begin("drums", false); // Modo escrita

        char k[6];

        

        if (type == "TH") { thresholds[pad] = val; sprintf(k, "t%d", pad); prefs.putInt(k, val); }

        else if (type == "GN") { gains[pad] = val; sprintf(k, "g%d", pad); prefs.putInt(k, val); }

        else if (type == "NA") { midiNotes[pad] = val; sprintf(k, "n%d", pad); prefs.putInt(k, val); }

        else if (type == "RT") { retrigger[pad] = val; sprintf(k, "r%d", pad); prefs.putInt(k, val); }

        else if (type == "CV") { curve[pad] = val; sprintf(k, "c%d", pad); prefs.putInt(k, val); }

        else if (type == "HHO") { openHHNote = val; prefs.putInt("hho", val); }

        else if (type == "HHC") { closedHHNote = val; prefs.putInt("hhc", val); }

        else if (type == "HHF") { footHHNote = val; prefs.putInt("hhf", val); }

        

        prefs.end();

      }

    }

  }



  // 3. Processamento dos Pads Piezo

  for (int i = 0; i < 12; i++) {

    int val = analogRead(padPins[i]);

    

    if (val > thresholds[i] && !padActive[i]) {

      if (millis() - lastHitTime[i] > retrigger[i]) {

        int baseVelocity = map(val, thresholds[i], 4095, 20, 127) * (gains[i] / 5.0);

        int velocity = applyCurve(baseVelocity, curve[i]);

        

        // Seleção dinâmica da nota para o Chimbal (GPIO 12)

        int noteToSend = midiNotes[i];

        if (i == hhPadIndex) {

          noteToSend = pedalPressed ? closedHHNote : openHHNote;

        }



        Serial.print("HIT:"); 

        Serial.print(i); 

        Serial.print(":"); 

        Serial.print(velocity); 

        Serial.print(":"); 

        Serial.println(noteToSend);

        

        padActive[i] = true;

        lastHitTime[i] = millis();

      }

    } else if (val < (thresholds[i] - 100)) {

      padActive[i] = false;

    }

  }

}