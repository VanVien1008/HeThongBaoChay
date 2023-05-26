#include <SoftwareSerial.h>
#include <SerialCommand.h>

#define Rx 11
#define Tx 12
SoftwareSerial mySerial(Rx, Tx);
SerialCommand SCmd;

int nutBom = 6;
int nutTat = 5;
int led = 4;
int mayBom = 8;
int LM35 = A0;
int camBienKhoi = 9;
int val_LM35;
float Vol;
float TempC;
float TempF;
int giaTriGas;
char TempC_c[3];
char message[14];
int coi = 7;
long long startTimeAuTo = 0;
long long startTimeBom = 0;

bool trangThaiMayBomCu = false;
bool trangThaiNhietDoCu = false;
bool trangThaiKhiGasCu = false;
bool trangThaiCanhBaoCu = false;
bool trangThaiMayBomMoi = false;
bool trangThaiNhietDoMoi = false;
bool trangThaiKhiGasMoi = false;
bool trangThaiCanhBaoMoi = false;


boolean ledstate = 0;
boolean buttonstate = 1;

long long int startTime;

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);

  SCmd.addCommand("batMayBom", buttonBatMayBom);
  SCmd.addCommand("tatMayBom", buttonTatMayBom);

  pinMode(coi, OUTPUT);
  pinMode(LM35, INPUT);
  pinMode(mayBom, OUTPUT);
  pinMode(camBienKhoi, INPUT);
  pinMode(nutTat, INPUT_PULLUP);
  pinMode(nutBom, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  digitalWrite(coi, HIGH);
  digitalWrite(mayBom, HIGH);
}

void setUpBanDau() {
  trangThaiCanhBaoMoi = false;
  trangThaiKhiGasMoi = false;
  trangThaiNhietDoMoi = false;
  trangThaiMayBomMoi = false;
  startTimeBom = 0;
  startTimeAuTo = 0;
}

int nhietDo(int LM35) {
  val_LM35 = analogRead(LM35);
  Vol = ((val_LM35 * 5.0) / 1024.0);
  TempC = Vol * 100.0;
  return TempC;
}

void capNhatTrangThai(int LM35, int camBienKhoi) {
  if (digitalRead(mayBom) == HIGH) {
    trangThaiMayBomMoi = false;
  }

  else {
    trangThaiMayBomMoi = true;
  }

  if (nhietDo(LM35) > 50) {
    trangThaiNhietDoMoi = true;
    trangThaiCanhBaoMoi = true;
  }

  if (digitalRead(camBienKhoi) == 0) {
    trangThaiKhiGasMoi = true;
    trangThaiCanhBaoMoi = true;
  }
}

void camBien() {
  if (trangThaiCanhBaoCu == true) {
    digitalWrite(coi, LOW);
    digitalWrite(led, HIGH);


  } else {
    digitalWrite(coi, HIGH);
    digitalWrite(led, LOW);
  }
}

void truyenTrangThaiQuaESP() {
  if (trangThaiCanhBaoCu != trangThaiCanhBaoMoi) {
    trangThaiCanhBaoCu = trangThaiCanhBaoMoi;
    if (trangThaiCanhBaoCu == 0) {
      mySerial.println("trangThaiCanh 0");
    } else {
      mySerial.println("trangThaiCanh 1");
    }
  }

  if (trangThaiKhiGasCu != trangThaiKhiGasMoi) {
    trangThaiKhiGasCu = trangThaiKhiGasMoi;
    if (trangThaiKhiGasCu == 0) {
      mySerial.println("trangThaiGas 0");
    } else {
      mySerial.println("trangThaiGas 1");
    }
  }

  if (trangThaiNhietDoCu != trangThaiNhietDoMoi) {
    trangThaiNhietDoCu = trangThaiNhietDoMoi;
    if (trangThaiNhietDoCu == 0) {
      mySerial.println("trangThaiNhiet 0");
    } else {
      mySerial.println("trangThaiNhiet 1");
    }
  }

  if (trangThaiMayBomCu != trangThaiMayBomMoi) {
    trangThaiMayBomCu = trangThaiMayBomMoi;
    if (trangThaiMayBomCu == 0) {
      mySerial.println("MayBom 0");
    } else {
      mySerial.println("MayBom 1");
    }
  }

  sprintf(TempC_c, "%d", nhietDo(LM35));
  strcpy(message, "ToC ");
  strcat(message, TempC_c);
  if (millis() - startTime > 200) {
    startTime = millis();
    mySerial.println(message);
  }
}

void tatBatMayBom() {
  if (digitalRead(nutBom) == LOW) {

    digitalWrite(mayBom, LOW);
  }

  if (digitalRead(nutTat) == LOW) {

    setUpBanDau();
    digitalWrite(mayBom, HIGH);
  }
}

void tatBatMayBomTuDong() {
  if (trangThaiNhietDoCu) {
    if (digitalRead(mayBom) == HIGH) {
      if (startTimeAuTo == 0) {
        startTimeAuTo = millis();
      } else {
        if (millis() - startTimeAuTo > 10000) {
          digitalWrite(mayBom, LOW);
        }
      }
    }
  }

  if (digitalRead(mayBom) == LOW) {
    if (startTimeBom == 0) {
      startTimeBom = millis();
    } else {
      if (millis() - startTimeBom > 30000) {
        digitalWrite(mayBom, HIGH);
        setUpBanDau();
      }
    }
  }
}


void tatCanhBao(int nutTat) {
  if (digitalRead(nutTat) == 0) {
    setUpBanDau();

    digitalWrite(nutTat, HIGH);
  }
}

void loop() {
  SCmd.readSerial();
  nhietDo(LM35);
  capNhatTrangThai(LM35, camBienKhoi);
  camBien();
  truyenTrangThaiQuaESP();
  tatBatMayBom();
  tatBatMayBomTuDong();


  if (mySerial.available()) {
    Serial.print(char(mySerial.read()));
  }
  Serial.print(digitalRead(camBienKhoi));
  Serial.print(trangThaiKhiGasMoi);
  Serial.println(trangThaiKhiGasCu);
  // Serial.println(nhietDo(LM35));

  // Serial.println(message);
  // Serial.println(digitalRead(nutTat));
}


void buttonBatMayBom() {
  int val;
  char *arg;
  arg = SCmd.next();
  val = atof(arg);
  digitalWrite(mayBom, LOW);
  Serial.println("Tat may bom thanh cong");
}

void buttonTatMayBom() {
  int val;
  char *arg;
  arg = SCmd.next();
  val = atof(arg);
  digitalWrite(mayBom, HIGH);
}