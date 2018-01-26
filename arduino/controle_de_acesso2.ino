#include <SPI.h>
#include <Ethernet.h>
#include "RestClient.h"
#include <MFRC522.h>
#include <ArduinoJson.h>

/* DEFINITIONS */
#define versionNumber 2    // software version
#define subversionNumber 3 // software subversion
#define SS_PIN 9           // pin for MFRC522 card reader
#define RST_PIN 5          // pin for MFRC522 card reader
#define speakerPin A0      // pin for speaker
#define relay A1           // pin for door relay
#define successTone 2960   // success tone
#define errorTone 415      // error tone
#define accessDelay 3000   // default delay time

int successRead;     // Variable integer to keep if we have Successful Read from Reader
byte readCard[4];    // Stores scanned ID read from RFID Module
String response;

///  ETHERNET CONFIGURATIONS ///
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x2A, 0x0A };
byte ip[] = { 192, 168, 0, 20 };
byte gateway[] = { 192, 168, 0, 1 };
byte subnet[] = { 255, 255, 255, 0 };

///  LOCK CONFIGURATIONS ///
boolean isSoundEnabled = false;
char* serverAddr = "192.168.0.106";
int serverPort = 8080;
char* authToken= "c29tZXRoaW5nOnNlY3JldA==";
char* accessCode = "904719c71ed45b04abc49363b5b38cc4"; // variable to store the lock unique id

///  START UP INTERFACES ///
RestClient client = RestClient(serverAddr,serverPort);
MFRC522 mfrc522(SS_PIN, RST_PIN);

//////////////////////

void setup(){

  // start serial port
  Serial.begin(9600);

  // start ethernet card
  Ethernet.begin(mac, ip, gateway, subnet);

  // start spi bus
  SPI.begin();

  // start rfid reader
  mfrc522.PCD_Init();
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

  // run rfid diagose
  runRfidDiags();

  /* CONFIG DE PINOS */
  pinMode(speakerPin, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  
  displayWelcomeMessage();
  
}


void loop(){
  
  do{
    successRead = getID();
  }while(!successRead);
  
  printID();
  
  int apiResponse = makeApiRequest();
  
  if(apiResponse == 200){
    accessGranted();
  }else{
    accessDenied();
  }
  
  delay(1000);
  
  displayWelcomeMessage();
  
}

