#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

SerialCommand SCmd;

#define Rx D0
#define Tx D1
SoftwareSerial mySerial(Rx, Tx);

const byte LED = D3;


#define WIFI_SSID "PTIT.HCM_SV"
#define WIFI_PASS ""
#define FIREBASE_HOST "canhbaochay-eb31b-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "78Rqw5ILw1GO1deQmYNxbXRP61UDMZHmAXOEQruv"

bool trangThaiNhiet;
bool trangThaiGas;
bool trangThai;
bool trangThaiMayBom;

String x;
String y;
String z;
float giaTriDoC;

FirebaseData firebaseData;
String path = "/";
FirebaseJson json;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  SCmd.addCommand("trangThaiGas", trangThaiKhiGasVal);
  SCmd.addCommand("trangThaiNhiet", trangThaiNhietDoVal);
  SCmd.addCommand("trangThaiCanh", trangThaiCanhBaoVal);
  SCmd.addCommand("ToC", giaTriNhietDo);
  SCmd.addCommand("MayBom", trangThaiMayBomVal);


  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path)) {
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }

  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {
  SCmd.readSerial();

  Firebase.setBool(firebaseData, path + "AppDieuKhien/trangThaiCanhBao", trangThai);

  Firebase.setBool(firebaseData, path + "AppDieuKhien/trangThaiNhietDo", trangThaiNhiet);

  Firebase.setBool(firebaseData, path + "AppDieuKhien/trangThaiKhiGas", trangThaiGas);

  Firebase.setFloat(firebaseData, path + "AppDieuKhien/giaTriNhietDo", giaTriDoC);

  Firebase.setBool(firebaseData, path + "AppDieuKhien/trangThaiMayBom", trangThaiMayBom);

  if (Firebase.getString(firebaseData, path + "AppDieuKhien/nutBatMayBom")) x = firebaseData.stringData();
  if (x == "1") {
    mySerial.println("batMayBom 1");
    Firebase.setString(firebaseData, path + "AppDieuKhien/nutBatMayBom", "0");
  }


  if (Firebase.getString(firebaseData, path + "AppDieuKhien/nutTatMayBom")) y = firebaseData.stringData();
  if (y == "1") {
    mySerial.println("tatMayBom 1");
    Firebase.setString(firebaseData, path + "AppDieuKhien/nutTatMayBom", "0");
  }

  if (Firebase.getString(firebaseData, path + "AppDieuKhien/nutTatHeThong")) z = firebaseData.stringData();
  if(z == "1"){
    mySerial.println("heThong 1");
  }
  if(z =="0"){
    mySerial.println("heThong 0");
  }

  if (mySerial.available()) {
    Serial.print(char(mySerial.read()));
  }
}

void trangThaiMayBomVal() {
  int val;
  char *arg;
  arg = SCmd.next();
  val = atoi(arg);
  if (val == 0) {

    trangThaiMayBom = false;
    Serial.println("trang thai may bom dc cap nhat");
  } else if (val == 1) {

    trangThaiMayBom = true;
    Serial.println("trang thai may bom dc cap nhat");
  }
}


void trangThaiCanhBaoVal() {
  int val;
  char *arg;
  arg = SCmd.next();
  val = atoi(arg);
  if (val == 0) {

    trangThai = false;
    Serial.println("trang thai canh bao dc cap nhat");
  } else if (val == 1) {

    trangThai = true;
    Serial.println("trang thai canh bao dc cap nhat");
  }
}
void trangThaiKhiGasVal() {
  int val;
  char *arg;
  arg = SCmd.next();
  val = atoi(arg);
  if (val == 0) {
    trangThaiGas = false;
    Serial.println("trang thai gas canh bao dc cap nhat");
  } else if (val == 1) {
    trangThaiGas = true;
    Serial.println("trang thai gas canh bao dc cap nhat");
  }
}
void trangThaiNhietDoVal() {
  int val;
  char *arg;
  arg = SCmd.next();
  val = atoi(arg);
  if (val == 0) {
    trangThaiNhiet = false;
    Serial.println("trang thai nhiet canh bao dc cap nhat");
  } else if (val == 1) {
    trangThaiNhiet = true;
    Serial.println("trang thai nhiet canh bao dc cap nhat");
  }
}

void giaTriNhietDo() {
  int val;
  char *arg;
  arg = SCmd.next();
  val = atof(arg);
  giaTriDoC = val;
}