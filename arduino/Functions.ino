
int getID() {
  if(!mfrc522.PICC_IsNewCardPresent()){
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()){
    return 0;
  }
  for(int i = 0; i < 4; i++){
    readCard[i] = mfrc522.uid.uidByte[i];
  }
  mfrc522.PICC_HaltA();
  beep();
  displayRead();
  return 1;
}


int printID(){
  
  if(Serial){
    Serial.println(" ");
    Serial.print("TAG UID: ");
  }
  
  for(int i = 0; i < 4; i++){
    if(Serial)
      Serial.print(readCard[i], HEX);
  }
  
  if(Serial)
    Serial.println(" ");
    
}


void runRfidDiags(){
  
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
    Serial.println("");
    // When 0x00 or 0xFF is returned, communication probably failed
    if ((v == 0x00) || (v == 0xFF)) {
      Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }

}


void outputApiResponse(){

  int str_len = response.length() + 1; 
  char json[str_len];
  response.toCharArray(json, str_len);

  // start json buffer
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(json);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  Serial.print("STATUS CODE: ");
  Serial.println((int)root["code"]);
  Serial.print("MESSAGE: ");
  Serial.println((const char*)root["message"]);
  
}


int makeApiRequest(){

  char tempChar[46];
  strcpy(tempChar, "Authorization: Basic ");
  strcat(tempChar, authToken);

  char str[80];
  strcpy(str, "/api");
  strcat(str, "/auth/");
  strcat(str, accessCode);
  strcat(str, "/");

  for(int i = 0; i < 4; i++){
    String stringOne = String(readCard[i], HEX);
    stringOne.toUpperCase();
    int str_len = stringOne.length() + 1; 
    char charBuf[str_len];
    stringOne.toCharArray(charBuf, str_len);
    strcat(str, charBuf);
  }

  client.setHeader(tempChar);
  int statusCode = client.get(str, &response);
  // some tests
  outputApiResponse();
  return statusCode;
}


void accessGranted(){
  displayAccessGranted();
  successBeep();
  openDoor();
}


void accessDenied(){
  displayAccessDenied();
  errorBeep();
}


void connectionFailed(){
  hardErrorBeep();
}


void fatalError(){
  hardErrorBeep();
}


void openDoor(){
  digitalWrite(relay, HIGH);
  delay(accessDelay);
  digitalWrite(relay, LOW);
}


void beep(){
  if(isSoundEnabled){
    digitalWrite(speakerPin, HIGH);
    delay(50);
    digitalWrite(speakerPin, LOW);
  }
}


void successBeep(){
  if(isSoundEnabled){
    digitalWrite(speakerPin, HIGH);
    delay(200);
    digitalWrite(speakerPin, LOW);
  }
}


void errorBeep(){
  if(isSoundEnabled){
    tone(speakerPin, errorTone);
    delay(300);
    noTone(speakerPin);
  }
}


void hardErrorBeep(){
  if(isSoundEnabled){
    for(int i = 0; i < 4; i++){
      tone(speakerPin, errorTone);
      delay(100);
      noTone(speakerPin);
      delay(50);
    }
  }
}


void displayWelcomeMessage(){
  Serial.println("");
  Serial.println("APROXIME A TAG DA LEITORA");
  response = "";
}


void displayRead(){
  if(Serial)
    Serial.println("TAG DETECTED!! ");
}


void displayAccessDenied(){
  if(Serial)
    Serial.println("ACCESS DENIED!");
}


void displayAccessGranted(){
  if(Serial)
    Serial.println("ACCESS GRANTED!");
}
