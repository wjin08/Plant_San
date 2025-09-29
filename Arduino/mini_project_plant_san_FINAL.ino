//MINI PROJECT PLANT-SAN

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define CDS_PIN A0
#define WATER_PIN A1



#define OLED_CS     10
#define OLED_DC     9
#define OLED_RESET  8

#define CMD_SIZE 60
#define ARR_CNT 5


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);
SoftwareSerial BTSerial(6, 7);  // Bluetooth 모듈: RX=6, TX=7

volatile bool timerIsrFlag = false;
unsigned long secCount = 0;

char sendBuf[CMD_SIZE] = {0};
char recvId[10] = "SWJ_SQL";  // 블루투스 송신자 ID

int sensorTime = 2;
int cds = 0;
int water = 0;
int happy = 0;

int avg_cds = 0;
int avg_water = 0;
int avg_happy = 0;

int total_cds = 0;
int total_water = 0;
int total_happy = 0;

int cnt=0;


void setup() {
  Serial.begin(115200);
  BTSerial.begin(9600);

  pinMode(CDS_PIN, INPUT);
  pinMode(WATER_PIN, INPUT);




  Timer1.initialize(1000000);  // 1초
  Timer1.attachInterrupt(timerIsr);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("OLED 초기화 실패"));
    while (1);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  if (BTSerial.available()) {
    bluetoothEvent();
  }

  if (timerIsrFlag) {
    timerIsrFlag = false;

    if (sensorTime > 0 && (secCount % sensorTime == 0)) {
      cds = analogRead(CDS_PIN);
      water = analogRead(WATER_PIN);
      cds = map(cds, 0, 1023, 0, 100);
      happyness();  // OLED 표정 출력
      total_cds += cds;
      total_water += water;
      cnt++;
      // Serial.print("cds : ");
      Serial.println(cds);
       //Serial.print(", water : ");
      Serial.println(water);
     // Serial.print(", happyness : ");
      Serial.println(happy);
    //  Serial.println(cnt);
    }
    //if(cnt >= 30 * 1)
    if(cnt >= 30)
    {
      sendBuf[CMD_SIZE] = {0};
      avg_cds = total_cds / 30 ; //30
      avg_water = total_water / 30; //30
      avg_happy = total_happy / 30; //30
      
      sprintf(sendBuf, "[%s]SENSOR@%d@%d@%d\n", recvId, avg_cds, avg_water, avg_happy);
      //sprintf(sendBuf, "[%s]SENSOR@%.1f@%.1f@%.1f\n", recvId, avg_cds, avg_water, avg_happy);
      BTSerial.write(sendBuf);
      total_cds = 0;
      total_water = 0;
      total_happy = 0;
      cnt = 0;
    }
    //sprintf(sendBuf, "[%s]SENSOR@%d@%d@%d\n", recvId, cds, water, happy);
    //BTSerial.write(sendBuf);
  }

#ifdef DEBUG
  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }
#endif
}

void timerIsr() {
  timerIsrFlag = true;
  secCount++;
}

void bluetoothEvent() {
  int i = 0;
  char *pToken;
  char *pArray[ARR_CNT] = {0};
  char recvBuf[CMD_SIZE] = {0};

  int len = BTSerial.readBytesUntil('\n', recvBuf, sizeof(recvBuf) - 1);
  pToken = strtok(recvBuf, "[@]");
  while (pToken != NULL) {
    pArray[i] = pToken;
    if (++i >= ARR_CNT) break;
    pToken = strtok(NULL, "[@]");
  }

  if (!strcmp(pArray[1], "GETSENSOR")) {
    if (pArray[2] == NULL) {
      sensorTime = 0;
    } else {
      sensorTime = atoi(pArray[2]);
      strcpy(recvId, pArray[0]);  // 발신자 ID 저장
    }
    // 현재 상태 즉시 응답
    sprintf(sendBuf, "[%s]SENSOR@%d@%d@%d\n", pArray[0], cds, water, happy);
    BTSerial.write(sendBuf);
  }
}

// ----------- OLED 관련 함수 ------------
void drawHappy() {
  display.clearDisplay();

  display.drawLine(36, 22, 39, 16, SSD1306_WHITE);
  display.drawLine(39, 16, 42, 12, SSD1306_WHITE);
  display.drawLine(42, 12, 46, 10, SSD1306_WHITE);
  display.drawLine(46, 10, 50, 12, SSD1306_WHITE);
  display.drawLine(50, 12, 54, 16, SSD1306_WHITE);
  display.drawLine(54, 16, 58, 22, SSD1306_WHITE);

  // 오른쪽 눈 (x좌표 그대로, y좌표 -2)
  display.drawLine(71, 22, 74, 16, SSD1306_WHITE);
  display.drawLine(74, 16, 77, 12, SSD1306_WHITE);
  display.drawLine(77, 12, 81, 10, SSD1306_WHITE);
  display.drawLine(81, 10, 85, 12, SSD1306_WHITE);
  display.drawLine(85, 12, 89, 16, SSD1306_WHITE);
  display.drawLine(89, 16, 93, 22, SSD1306_WHITE);
  // 입 (soso 스타일 유지)
  display.drawLine(50, 38, 56, 43, SSD1306_WHITE);
  display.drawLine(56, 43, 72, 43, SSD1306_WHITE);
  display.drawLine(72, 43, 78, 38, SSD1306_WHITE);

display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 55);
  display.print("PLANT-SAN IS HAPPY~!");

  display.display();
}








void drawSoso() {
  display.clearDisplay();

  // 눈 간격 2씩 벌리기 (왼쪽 49, 오른쪽 79)
  display.fillCircle(49, 17, 4, SSD1306_WHITE);  // 반지름 4로 변경
  display.drawCircle(49, 17, 9, SSD1306_WHITE);

  display.fillCircle(79, 17, 4, SSD1306_WHITE);  // 반지름 4로 변경
  display.drawCircle(79, 17, 9, SSD1306_WHITE);

  // 입 세로 좌표 2씩 올리기
  display.drawLine(50, 38, 56, 43, SSD1306_WHITE);
  display.drawLine(56, 43, 72, 43, SSD1306_WHITE);
  display.drawLine(72, 43, 78, 38, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 55);
  display.print("PLANT-SAN");

  display.display();
}

void drawSunshine() {
  display.clearDisplay();

  // 얼굴 전체를 왼쪽으로 14, 아래로 14 이동 (8 + 6)
  display.drawLine(33 - 14, 10 + 14, 57 - 14, 17 + 14, SSD1306_WHITE);
  display.drawLine(57 - 14, 17 + 14, 33 - 14, 24 + 14, SSD1306_WHITE);

  display.drawLine(75 - 14, 17 + 14, 99 - 14, 10 + 14, SSD1306_WHITE);
  display.drawLine(99 - 14, 24 + 14, 75 - 14, 17 + 14, SSD1306_WHITE);

  display.drawLine(50 - 14, 38 + 14, 56 - 14, 43 + 14, SSD1306_WHITE);
  display.drawLine(56 - 14, 43 + 14, 72 - 14, 43 + 14, SSD1306_WHITE);
  display.drawLine(72 - 14, 43 + 14, 78 - 14, 38 + 14, SSD1306_WHITE);

  display.drawLine(115, 10, 95, 25, SSD1306_WHITE);   // 중앙
  display.drawLine(117, 10, 97, 30, SSD1306_WHITE);   // 조금 아래쪽
  display.drawLine(113, 10, 93, 20, SSD1306_WHITE);   // 조금 위쪽
  display.drawLine(119, 10, 99, 35, SSD1306_WHITE); 

  display.display();
}





void drawSad() {
  display.clearDisplay();

  // 왼쪽 눈 - x좌표 총 왼쪽으로 6, y좌표 위로 3
  display.drawLine(36, 17, 56, 17, SSD1306_WHITE);  // 가로선 (x -2)
  display.drawLine(42, 17, 42, 28, SSD1306_WHITE);  // 세로선 1 (x -2)
  display.drawLine(50, 17, 50, 28, SSD1306_WHITE);  // 세로선 2 (x -2)

  // 오른쪽 눈 - x좌표 총 왼쪽으로 6, y좌표 위로 3
  display.drawLine(72, 17, 92, 17, SSD1306_WHITE);  // 가로선 (x -2)
  display.drawLine(78, 17, 78, 28, SSD1306_WHITE);  // 세로선 1
  display.drawLine(86, 17, 86, 28, SSD1306_WHITE);  // 세로선 2

  // 슬픈 입 (y 좌표 3픽셀 위로)
  display.drawLine(50, 42, 56, 37, SSD1306_WHITE);
  display.drawLine(56, 37, 72, 37, SSD1306_WHITE);
  display.drawLine(72, 37, 78, 42, SSD1306_WHITE);


display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 55);
  display.print("PLANT-SAN SAD");

  display.display();
}




void happyness() {
  bool isWaterDetected = water > 300;
  bool isBright = cds > 25;

  if (isWaterDetected && isBright) {
    happy = 2;
    drawHappy();
  } else if (isWaterDetected || isBright) {
    happy = 1;

    if (isWaterDetected && !isBright) {
      drawSoso();     
    } else if (!isWaterDetected && isBright) {
      drawSunshine();  
    }

  } else {
    happy = 0;
    drawSad();
  }
  total_happy += happy;
}
