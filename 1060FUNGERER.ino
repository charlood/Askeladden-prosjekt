int Pluss = 3; // grønn er pluss
int Minus = 5;
int Reset = 2;
int Bekreft = 4;
int RepRegistering = A0;

int buttonState = 0;

int treningsaakt[3] = {0, 0, 0}; // sett, repetisjon, pause
bool isStarted = true;
bool erSettRegistert = false;
bool erRepRegistert = false;
bool erPauseRegistert = false;
bool erAltRegistert = false;
bool erDetPause = false;
int pin;
int antallRepGjort = 0;
int midlertidigRep = 0;
int naaPause = 0;
int currentMillis = 0;
int lastButtonDebounceTime = 0;
int previous = 0;
bool bypass = false;


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


#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int ledPin = 13;


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
  // put your setup code here, to run once:
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.clear();

  BLE.begin(); // starte  bluetooth modul i intel curie chip
  BLE.scan(); // starte å skanne etter engheter
  slaveFinder(); // metode som venter til den finner en spesifisert characteristikk som sensor holder og broadcaster
  delay(2000); // brukt under testing

}

void loop() {
  //pauseLys();

  // put your main code here, to run repeatedly:
  resetKnapp();
  if (erAltRegistert) {
    feedbackTreningMsensor();
    //feedbackTrening();
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



// brukes ikke men beholdes for  testing
void feedbackTrening() {
  midlertidigRep = treningsaakt[1];
  if (treningsaakt[0] != 0) {
    if (treningsaakt[1] != 0) {
      treningsLys();
      lcd.clear();
      while (antallRepGjort != treningsaakt[1]) {
        resetKnapp();
        updateCell(0, 0, "Rep igjen: " + (String)midlertidigRep);
        //lcd.print("Rep igjen : " + String(midlertidigRep));
        erKnappTrykketRegistering(RepRegistering);
      }
      feedbackPause();
    }
  } else {
    Serial.println("Ferdig");
    resetKnapp();
    skruAv();
  }

}

// feedback med sensor som kontaktpunkt
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

// boolsk evaluator for om sensor sin verdi er oppdatert eller ikke
boolean sensorValueUpdated() {

  //antallRepGjort = antallRepGjort + 1;
  //midlertidigRep = midlertidigRep - 1;
  if (sensorCharacteristic.valueUpdated()) {
    return true;
  } else {
    return false;
  }
}



// teller ned pause for bruker
void feedbackPause() {
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
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 2, 0)); // GRØNN.
    pixels.show();
  }
}

void treningsLys() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0 , 2));
    pixels.show();
  }
}



// Signalisering for  bruker
void lysBlink(int farge1, int farge2, int farge3) {
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


void regnbue() {

  //lag regnbue m/ringen
}

// brukes ikke, vil skape flimring pga konstant oppdatering 
void updateDisplay() {
  lcd.clear();
}


// resetting av terminal
void resetKnapp() {
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







