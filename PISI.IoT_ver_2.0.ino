#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Adafruit_MLX90614.h>
#include <Wire.h>
#include <Adafruit_Fingerprint.h>

#define DonVi 1
#define Chuc 10
#define CauThoai 2
#define TempMin 0
#define TempMax 100

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
SoftwareSerial mySerial(2, 3);
DFRobotDFPlayerMini myDFPlayer;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

double ambientTempC, objectTempC;
int btnPress = 4;

void setup() {
  // put your setup code here, to run once:
  
  
  mlx.begin();
  Serial.begin(115200);
  Serial.println("Initializing DFPlayer ... (May take 3~5 seconds)");
  
  startConnectToPlayer();
  myDFPlayer.volume(15);  //Set volume value. From 0 to 30

  startConnectToFingerprint();

  pinMode(btnPress, INPUT);
}

void loop() {

  int user = waitForDetectFingerprint();
  
  objectTempC = mlx.readObjectTempC();
  ambientTempC = mlx.readAmbientTempC();
  
  Serial.println(objectTempC);
  myDFPlayer.readState();
  readNumber(objectTempC);
}

int waitForDetectFingerprint(){
  
  startConnectToFingerprint();
  int IdUser = -1;
  while(IdUser == -1){
    IdUser = getFingerprintIDez();
    Serial.println(IdUser);
    delay(500);
  }
  return IdUser;
}

void startConnectToPlayer(){
  mySoftwareSerial.begin(9600);
  while (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    delay(1000);
  }
  Serial.println("DFPlayer Mini online.");
}

void startConnectToFingerprint(){
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}

void readNumber(double num){
  startConnectToPlayer();
  if(num < TempMin) excuteRead(CauThoai, 3);        //003smailler20.mp3
  else if (num > TempMax) excuteRead(CauThoai, 4);  //004bigger90.mp3)
  else {
    excuteRead(CauThoai, 1);                        //nhiet do co the cua ban la.mp3
    
    docSoHangChuc(num);                             //read number
    if (num - (int)num > 0){
      docSoHangChuc((num - (int)num)*100);
    }
    excuteRead(CauThoai, 2);                        //do C.mp3
  }
}

void docSoHangChuc(int num){
  if (num/10 > 0){
    excuteRead(Chuc, num/10);
    docSoDonVi(num%10);
  } else
    docSoDonVi(num);
}

void docSoDonVi(int num){
  if (num > 0){
    excuteRead(DonVi, num);
  }
}

void excuteRead(int folder, int index){
  myDFPlayer.playFolder(folder, index);
  waitForPlayingFinish();
}

void waitForPlayingFinish(){
  int detail = 0;
  while (detail != DFPlayerPlayFinished){
    if (myDFPlayer.available()) 
      detail = getDetail(myDFPlayer.readType(), myDFPlayer.read());
  }
}

int getDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      return TimeOut;
      break;
    case WrongStack:
//      Serial.println(F("Stack Wrong!"));
      return WrongStack;
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      return DFPlayerCardInserted;
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      return DFPlayerCardRemoved;
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      return DFPlayerCardOnline;
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      return DFPlayerUSBInserted;
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      return DFPlayerUSBRemoved;
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      return DFPlayerPlayFinished;
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      return DFPlayerError;
      break;
    default:
      return -1;
      break;
  }
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}

double findMod(double a, double b) { 
    // Handling negative values 
    if (a < 0) 
        a = -a; 
    if (b < 0) 
        b = -b; 
  
    // Finding mod by repeated subtraction 
    double mod = a; 
    while (mod >= b) 
        mod = mod - b;
    return mod; 
}
