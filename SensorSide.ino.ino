// AutoCalibrate Laant av Adafruit
//https://learn.adafruit.com/adafruit-analog-accelerometer-breakouts/calibration-and-programming;

boolean varIBevegelse = false;

static int antallTester = 0;

//Accelerometer max G-krefter verdi;
int scale = 200;

//Analoge Pins
const int akseX = A0; // akseX port på accelerometer
const int akseY = A1;
const int akseZ = A2;

//Kalibrerings Verdier lavest(min) og høyest(max) G;
int xRawMin = 512; //minste rå verdi fra accelerometer
int xRawMax = 352; //største rå verdid 

int yRawMin = 512;
int yRawMax = 352;

int zRawMin = 512;
int zRawMax = 352;

//Andre variabler for kalibrering

int sampleSize = 10;                 //antall lesninger på sensor akser (ReadAxis)
int kalibTid = 5000;                // kalibreringsTid for AutoCalibrate
boolean kalibreringFerdig = false; // false inntil kalibrering er ferdig;



//__________________Bluetooth___________________________

// variables:
boolean centralConnected = false;
boolean subbed = false;

#include <SPI.h>
#include <BLEPeripheral.h>


BLEPeripheral slave = BLEPeripheral();

BLEService sensorService = BLEService("19b10000e8f2537e4f6cd104768a1214");

BLECharCharacteristic sensorCharacteristic = BLECharCharacteristic("19b10001e8f2537e4f6cd104768a1214", BLERead | BLENotify | BLEWrite);



void setup() {
  pinMode(5, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);

  //____Accelerometer pinModus;
  pinMode(akseX, INPUT);
  pinMode(akseY, INPUT);
  pinMode(akseZ, INPUT);

  TotalKalibrering();

  slave.setLocalName("Slave");
  slave.setAdvertisedServiceUuid(sensorService.uuid());
  sensorCharacteristic.setValue(0);

  slave.addAttribute(sensorService);
  slave.addAttribute(sensorCharacteristic);

  // Event handlers -- central connected / central not connected
  slave.setEventHandler(BLEConnected, slaveConnectHandler);
  slave.setEventHandler(BLEDisconnected, slaveDisconnectHandler);

  // characteristic event handlers
  sensorCharacteristic.setEventHandler(BLESubscribed, characteristicSubscribed);
  sensorCharacteristic.setEventHandler(BLEUnsubscribed, characteristicUnsubscribed);

  slave.begin();

}



void loop() {
  BLECentral central = slave.central();
  SensorReadX(analogRead(akseX));
  //Serial.println(analogRead(akseX));
  
  
}



//Sjekker aksen for akselerometer x antall ganger gitt i sampleSize (Lånt fra Adafruit eksempel)
int ReadAxis(int axisPin) {
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading / sampleSize;
}


void AutoCalibrate(int xRaw, int yRaw, int zRaw) {
  if (xRaw < xRawMin)
  {
    xRawMin = xRaw;
  }

  if (xRaw > xRawMax)
  {
    xRawMax = xRaw;
  }

  if (yRaw < yRawMin)
  {
    yRawMin = yRaw;
  }
  if (yRaw > yRawMax)
  {
    yRawMax = yRaw;
  }

  if (zRaw < zRawMin)
  {
    zRawMin = zRaw;
  }
  if (zRaw > zRawMax)
  {
    zRawMax = zRaw;
  }
}


unsigned long int sensorRead(int axisPin) {
  unsigned long int x = analogRead(axisPin);
  return x;
}


void SensorReadX(unsigned long int xyz) {
  // implementer heller Gjennomsnittr slik at vi sjekker mot gjennomsnitt av 5 forrige og nåværende verdi i forhold til dette gjennomsnittet.
  if (varIBevegelse) {

    if ( xyz < xRawMin ) {
      //sensorCharacteristic.setValue("01");  // mulig feil plassering av repetisjons registrering
      Serial.println("var i bevegelse");
      Serial.println("xyz i bevegelse");
      Serial.println(xyz);
      Serial.println("\n");
      Serial.println("var i bevegelse");
      //Serial.end();
      varIBevegelse = !varIBevegelse;
    }
  }
  else {
    if (xyz >= xRawMax) {
      sensorCharacteristic.setValue("00");
      Serial.println("var ikke i bevegelse");
      Serial.println("xyz ikke i bevegelse");
      Serial.println(xyz);
      Serial.println(antallTester);
      Serial.println("\n");
      //Serial.end();
      antallTester++;
      varIBevegelse = !varIBevegelse;
    }
  }
}


void TotalKalibrering() {
  while (millis() < kalibTid) {
    int xRaw = ReadAxis(akseX);
    int yRaw = ReadAxis(akseY);
    int zRaw = ReadAxis(akseZ);
    AutoCalibrate(xRaw, yRaw, zRaw);
    Serial.println("kalibrerer");
    Serial.println(minMax(xRawMin, xRawMax, "X"));
    Serial.println(minMax(yRawMin, yRawMax, "Y"));
    Serial.println(minMax(zRawMin, zRawMax, "Z"));
  }
}


// kan fjernes, kun nødvendig ved testing.
String  minMax(int minst, int stoerst, String akse) {
  String m = akse + "-min:";
  String s = akse + "-max:";
  String xmin = m + minst;
  String xmax = s + stoerst;
  String retning = xmin + " <--min  max -->   " + xmax;
  return retning;
}


// bluetooth_metoder__og__funksjoner______________________________________


void slaveConnectHandler(BLECentral& central) { // central connected event handler
  centralConnected = true;
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}


void slaveDisconnectHandler(BLECentral& central) { // central disconnected event handler
  centralConnected = false;
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}

void characteristicSubscribed(BLECentral& central, BLECharacteristic& characteristic) { // characteristic subscribed event handler
  subbed = true;
  Serial.println(F("Characteristic event, subscribed"));
}

void characteristicUnsubscribed(BLECentral& central, BLECharacteristic& characteristic) {   // characteristic unsubscribed event handler
  subbed = false;
  Serial.println(F("Characteristic event, unsubscribed"));
}







