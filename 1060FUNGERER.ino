int Pluss = 3; 
int Minus = 5;
int Reset = 2;
int Bekreft = 4;
int RepRegistering = A0;
//Setter opp pins til knapper og sensor


int treningsaakt[3] = {0, 0, 0}; // sett, repetisjon, pause
bool isStarted = true;
bool erSettRegistert = false;
bool erRepRegistert = false;
bool erPauseRegistert = false;
bool erAltRegistert = false;
bool erDetPause = false;
//setter opp booleans som brukes til å sjekke om bruker-inputt er registrert

int pin;
int antallRepGjort = 0;
int midlertidigRep = 0;
int naaPause = 0;
int currentMillis = 0;
int lastButtonDebounceTime = 0;
int previous = 0;
//setter opp variabler som blir brukt under selve treningsøkten

int currentTime = 0;

// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
unsigned long previousMillis = 0;
const long interval = 600;
bool lyser = false;


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            13

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      24

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


// Include the Neopixel, servo and button library
#include <Adafruit_NeoPixel.h>
const byte neoPin = 13;
const byte neoPixels = 24;
byte neoBright = 100;

//Koden over er hentet fra Adafruits Neopixl-bibliotek

#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int ledPin = 13;

//Koden over setter opp LCD-skjermen


unsigned long interval2 = 200;
unsigned long tidels = 0;
unsigned long secs = 0;
unsigned long mins = 0;


#include <CurieBLE.h>
BLEDevice slave;
BLECharacteristic sensorCharacteristic;
boolean found = false;



void setup() {

  pinMode(RepRegistering, INPUT);
  pinMode(Pluss, INPUT);
  pinMode(Minus, INPUT);
  pinMode(Reset, INPUT);
  pinMode(Bekreft, INPUT);
  pinMode(ledPin, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(1, 0);
  Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.clear();
  //Setter opp alle pins som skal brukes til input eller output

  BLE.begin(); // starte  bluetooth modul i intel curie chip
  BLE.scan(); // starte å skanne etter engheter
  slaveFinder(); // metode som venter til den finner en spesifisert characteristikk som sensor holder og broadcaster
  delay(2000); // brukt under testing

}

void loop() {

  //I loopen går man igjennom og sjekker hele tiden om Resett blir trykket på
  //Ellers så går man igjennom menyen og tar imot input
  //Når all input er registrert begynner man med treningsøkten
  resetKnapp();
  if (erAltRegistert) {
    feedbackTreningMsensor();
    resetKnapp();
  } else {
    resetKnapp();
    registrering();
  }

  
}

// kode for registrering av trenings opplegg
void registrering() {
  pixels.clear();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0 , 0));
    pixels.show();
  }

  //Her går man igjen omg registrerer input om antall sett, repitsjoner og pause
  while (!erSettRegistert) {
    resetKnapp();
    erKnappTrykketBekreftSett();
    int antallSett = treningsaakt[0];
    updateCell(0, 0, "Ant sett: " + (String)antallSett);
    //Serial.println("Ant Sett: " +  String(antallSett));
    erKnappTrykketPluss(0, 1);
    erKnappTrykketMinus(0, 1);
  }
  lcd.clear();
  while (!erRepRegistert) {
    resetKnapp();
    erKnappTrykketBekreftRep();
    int antallRep = treningsaakt[1];
    updateCell(0, 0, "Ant Rep: " + String(antallRep));
    //Serial.println("Ant Rep: " +  String(antallRep));
    erKnappTrykketPluss(1, 1);
    erKnappTrykketMinus(1, 1);
  }
  lcd.clear();
  while (!erPauseRegistert) {
    resetKnapp();
    erKnappTrykketBekreftPause();
    int antallPause = treningsaakt[2];
    //updateDisplay();
    //lcd.print("Pause: " +  String(antallPause));
    updateCell(0, 0, "Pause: " + String(antallPause));
    Serial.println("Pause: " +  String(antallPause));
    // Hvordan fikser vi det slik at det blir 1:15 og ikke 75 sekunder?
    erKnappTrykketPluss(2, 10);
    erKnappTrykketMinus(2, 10);
  }
  erAltRegistert = true;
}

void erKnappTrykketPluss(int hvilken, int aak) {
  resetKnapp();
  currentTime = millis();
  if (digitalRead(Pluss)) {
    if (currentTime - previousMillis >= interval) {
      previousMillis = currentTime;
      int settnoe = treningsaakt[hvilken];
      treningsaakt[hvilken] = settnoe + aak;
    }
  }
}

void erKnappTrykketMinus(int hvilken, int mink) {
  resetKnapp();
  currentTime = millis();
  if (digitalRead(Minus)) {
    if (currentTime - previousMillis >= interval) {
      previousMillis = currentTime;
      int settnoe = treningsaakt[hvilken];
      if (treningsaakt[hvilken] > 0) {
        treningsaakt[hvilken] = settnoe - mink;
      }
    }
  }

}

void erKnappTrykketBekreftSett() {
  resetKnapp();
  currentTime = millis();
  if (digitalRead(Bekreft)) {
    if (currentTime - previousMillis >= interval) {
      previousMillis = currentTime;
      erSettRegistert = true;
    }
  }

}

void erKnappTrykketBekreftRep() {
  resetKnapp();
  currentTime = millis();
  if (digitalRead(Bekreft)) {
    if (currentTime - previousMillis >= interval) {
      previousMillis = currentTime;
      erRepRegistert = true;
    }
  }
}


void erKnappTrykketBekreftPause() {
  resetKnapp();
  currentTime = millis();
  if (digitalRead(Bekreft)) {
    if (currentTime - previousMillis >= interval) {
      previousMillis = currentTime;
      erPauseRegistert = true;
    }
  }
}

void erKnappTrykketRegistering(int pin) {
  currentTime = millis();
  if (digitalRead(pin)) {
    if (currentTime - previousMillis >= interval) {
      previousMillis = currentTime;
      antallRepGjort = antallRepGjort + 1;
      midlertidigRep = midlertidigRep - 1;
    }
  }
}




void feedbackTreningMsensor() {
  midlertidigRep = treningsaakt[1];
  if (treningsaakt[0] != 0) {
    if (treningsaakt[1] != 0) {
      treningsLys();
      lcd.clear();
      while (antallRepGjort != treningsaakt[1]) {
        resetKnapp();
        slaveSubscribe();
        BLE.poll();
        updateCell(0, 0, "Rep igjen: " + (String)midlertidigRep);
        //lcd.print("Rep igjen : " + String(midlertidigRep));
        if (sensorValueUpdated()) {
          antallRepGjort = antallRepGjort + 1;
          midlertidigRep = midlertidigRep - 1;
        }
      }
      slaveUnsubscribe();
      feedbackPause();
    }
  } else {
    Serial.println("Ferdig");
    resetKnapp();
    skruAv();
  }

}

boolean sensorValueUpdated() {

  //antallRepGjort = antallRepGjort + 1;
  //midlertidigRep = midlertidigRep - 1;
  if (sensorCharacteristic.valueUpdated()) {
    return true;
  } else {
    return false;
  }
}




void feedbackPause() {
  //Denne funksjonen brukes til å telle ned pausen og gi brukeren feedback via skjerm og en ring som lyser/blinker grønt

  int interval2 = 1000;
  antallRepGjort = 0;
  if (treningsaakt[0] > 0) {
    int nySettIgjen = treningsaakt[0];
    nySettIgjen = nySettIgjen - 1;
    treningsaakt[0] = nySettIgjen;
    naaPause = treningsaakt[2];
    lcd.clear();
    while (naaPause > 0) {
      resetKnapp();
      //updateDisplay();
      //lcd.print("Pause: " + String(naaPause) + " sek");
      int tid = millis();
      if ((tid - previous) >= interval2) {
        previous = tid;
        naaPause = naaPause - 1;
      }
      if (naaPause > 10) {
        updateCell(0, 0, "Pause: " + String(naaPause));
        pauseLys();
      }
      if (naaPause <= 10) {
        updateCell(0, 0, "Pause: " + String(naaPause) + " ");
        lysBlink(0, 2, 0);
      }
    }

  }

}



void pauseLys() {
  //Lyser grønt
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 2, 0)); // GRØNN.
    pixels.show();
  }
}

void treningsLys() {
  //Lyser blått
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0 , 2));
    pixels.show();
  }
}




void lysBlink(int farge1, int farge2, int farge3) {
  //Blinker den fargen som man sender med som parameter
  //Mye kode hentet fra Adafruits Neopixel-bibliotek
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval2) {
    previousMillis = currentMillis;
    if (lyser == false) {
      for (int i = 0; i < NUMPIXELS; i++) {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(farge1, farge2, farge3)); // Moderately bright green color.
        pixels.show();
      }
      lyser = true;
    }
    if (lyser == true) {
      Serial.println("--------------------------------------------------------------------------");
      for (int i = 0; i < NUMPIXELS; i++) {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Moderately bright green color.
        pixels.show();
      }
      lyser = false;
    }
  }


}


void skruAv() {
  //Funksjon som brukes når treningsøkt er ferdig. Skjermen sier "Du er ferdig" i 5 sekunder
  //Samtidig så resetter den alle variablene til start og når de 5 sekundene er ferdig så kan man registrere en ny treningsøkt
  
  int interval3 = 5000;
  int previousTing = millis();
  if (isStarted) {
    int current = millis();
    if ((current - previousTing) >= interval3) {
      lcd.clear();
      treningsaakt[0] = 0;
      treningsaakt[1] = 0;
      treningsaakt[2] = 0;
      isStarted = true;
      erSettRegistert = false;
      erRepRegistert = false;
      erPauseRegistert = false;
      erAltRegistert = false;
      erDetPause = false;
      antallRepGjort = 0;
      midlertidigRep = 0;
      naaPause = 0;
    } else  {
      updateCell(0, 0, "Du er ferdig");
    }
  }
}


void updateDisplay() {
  //Oppdaterer displayet til LCD-skjermen
  
  lcd.clear();
}



void resetKnapp() {
  //Resetter alle variabler, slik de var i starten
  
  if (digitalRead(Reset)) {
    Serial.println("resett");
    lcd.clear();
    treningsaakt[0] = 0;
    treningsaakt[1] = 0;
    treningsaakt[2] = 0;
    erSettRegistert = false;
    erRepRegistert = false;
    erPauseRegistert = false;
    erAltRegistert = false;
    erDetPause = false;
    antallRepGjort = 0;
    midlertidigRep = 0;
    naaPause = 0;
  }
}

// oppdater lcd visning
void updateCell(int pos, int rad, String content) {
  lcd.setCursor(pos, rad);
  lcd.print(content);
}


// finn ønsket device
void slaveFinder() {
  while (!found ) {
    BLEDevice slave = BLE.available();
    if (slave) {
      Serial.println(slave.address());
      if (slave.localName() == "Slave") {
        BLE.stopScan();
        //Serial.println("fant slaven via IF statement");
        found = true;
        explorePeripheral(slave);

      }
    }
  }
}

// utforsk og finn attributter for Peripheral / sensor
void explorePeripheral(BLEDevice peripheral) {
  if (peripheral.connect()) {
    Serial.println("tilkoblet");
  }
  else {
    Serial.println("klarte ikke koble til");
  }

  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes found");
  }
  else {
    Serial.println("Attribute discovery failed");
  }

  Serial.println();
  Serial.print("Device name: ");
  Serial.println(peripheral.localName());

  // loop the services of the peripheral and explore each
  for (int i = 0; i < peripheral.serviceCount(); i++) {
    BLEService service = peripheral.service(i);
    serviceSearch(service);
  }
}

// søk etter gitt service
void serviceSearch(BLEService service) {
  for (int i = 0; i < service.characteristicCount(); i++) {
    BLECharacteristic characteristic = service.characteristic(i);
    characteristicSearch(characteristic);
  }
}

//  søk etter spesifisert characteristikk
void characteristicSearch(BLECharacteristic characteristic) {
  if (characteristic.canNotify()) {
    Serial.println(characteristic.uuid());
    sensorCharacteristic = characteristic;
  }
}

// abbonner på characeristic for å oppdage verdi endring fra accelerometer
void slaveSubscribe() {
  sensorCharacteristic.subscribe();
  //boolean subbed = true;
}

//av abbonner på characeristic for å ikke oppdage verdi endring i pauser
void slaveUnsubscribe() {
  sensorCharacteristic.unsubscribe();
  //boolean subbed = false;
}







