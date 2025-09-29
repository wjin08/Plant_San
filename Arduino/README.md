
## 소개

PLANT-SAN의 Aruduino 파트는 식물의 상태를 실시간으로 모니터링하고, 서버에 상태를 송신하며, OLED 디스플레이에 귀여운 표정으로 식물의 기분을 표현합니다.

**주요 기능:**
- 🌞 조도(빛) 감지
- 💧 수분(물) 감지
- 😊 식물 상태에 따른 4가지 표정 표시
- 📱 블루투스를 통한 실시간 데이터 전송
- 📊 센서 데이터 평균값 계산 및 전송

## 🔌 회로 연결

### 1. OLED 디스플레이 (SSD1306 - SPI 방식)
```
OLED → Arduino
-----------------
VCC  → 5V
GND  → GND
SCK  → D13 (SPI Clock)
SDA  → D11 (SPI MOSI)
CS   → D10 (Chip Select)
DC   → D9  (Data/Command)
RES  → D8  (Reset)
```

### 2. 조도 센서 (CdS)
```
CdS 센서 한쪽 → 5V
CdS 센서 다른쪽 → A0 핀 + 10kΩ 저항 → GND
```
(분압 회로 구성)

### 3. 토양 수분 센서
```
수분센서 VCC → 5V
수분센서 GND → GND
수분센서 AO  → A1
```

### 4. 블루투스 모듈 (HC-06)
```
HC-06 → Arduino
-----------------
VCC   → 5V
GND   → GND
TXD   → D6 (Arduino RX)
RXD   → D7 (Arduino TX)
```

### 회로도
```
        Arduino Uno
    ┌─────────────────┐
    │                 │
    │  D13 ─────── SCK (OLED)
    │  D11 ─────── SDA (OLED)
    │  D10 ─────── CS  (OLED)
    │  D9  ─────── DC  (OLED)
    │  D8  ─────── RES (OLED)
    │                 │
    │  A0  ─────── CdS 센서
    │  A1  ─────── 수분 센서
    │                 │
    │  D6  ─────── BT TXD
    │  D7  ─────── BT RXD
    │                 │
    │  5V  ─────── 전원 공급
    │  GND ─────── 공통 접지
    └─────────────────┘
```

## 📚 라이브러리 설치

Arduino IDE를 열고 다음 라이브러리를 설치하세요.

### 방법 1: 라이브러리 매니저 사용 (권장)

1. Arduino IDE 메뉴: `스케치` → `라이브러리 포함하기` → `라이브러리 관리`
2. 검색창에 라이브러리 이름 입력 후 설치

필요한 라이브러리:
- **Adafruit GFX Library** (by Adafruit)
- **Adafruit SSD1306** (by Adafruit)
- **TimerOne** (by Jesse Tane, Jérôme Despatis, Michael Polli, Dan Clemens, Paul Stof)

### 방법 2: 수동 설치

1. 각 라이브러리 GitHub에서 ZIP 파일 다운로드
2. `스케치` → `라이브러리 포함하기` → `.ZIP 라이브러리 추가`

**기본 포함 라이브러리** (별도 설치 불필요):
- SPI.h
- Wire.h
- SoftwareSerial.h

## 💻 코드 업로드

### 1단계: Arduino IDE 설정
1. Arduino IDE 실행
2. `도구` → `보드` → `Arduino Uno` 선택
3. `도구` → `포트` → 연결된 COM 포트 선택

### 2단계: 코드 복사 및 수정
1. 제공된 코드 전체를 Arduino IDE에 복사
2. 필요시 블루투스 ID 변경:
```cpp
char recvId[10] = "SWJ_SQL";  // 원하는 이름으로 변경 가능
```

### 3단계: 업로드
1. `스케치` → `확인/컴파일` 클릭 (에러 확인)
2. `스케치` → `업로드` 클릭
3. 업로드 완료 대기 (약 10초)

## 🎮 사용 방법

### 1. 기본 동작 확인

1. Arduino에 전원 연결 (USB 또는 외부 전원)
2. OLED 화면에 표정이 나타나는지 확인
3. 시리얼 모니터 열기: `도구` → `시리얼 모니터` (115200 baud)

### 2. 센서 테스트

**조도 센서 테스트:**
- 센서에 손으로 빛을 가리면 → 어두워짐 감지
- 밝은 곳에 두면 → 밝음 감지

**수분 센서 테스트:**
- 센서를 물에 담그거나 젖은 흙에 꽂으면 → 수분 감지
- 건조한 상태 → 수분 없음 감지

### 3. 블루투스 연결

**스마트폰 연결:**
1. 스마트폰 블루투스 켜기
2. "HC-06" 검색 및 페어링

**데이터 수신 형식:**
```
[SWJ_SQL]SENSOR@45@678@1
         └─┬─┘ └┬┘└┬┘└┬┘
           │    │  │  └─ 행복도 (0~2)
           │    │  └──── 수분값 (0~1023)
           │    └─────── 조도값 (0~100)
           └──────────── 전송 ID
```

**명령어 전송:**
```
[YOUR_ID]GETSENSOR@2
         └───┬───┘ └─ 전송 주기 (초)
             └────── 명령어
```

## 🎭 동작 원리

### 표정 상태 (4가지)

| 표정 | 조건 | 행복도 값 | 설명 |
|------|------|-----------|------|
| 😊 행복 (Happy) | 물 O + 빛 O | 2 | 물과 빛 모두 충분 |
| 😐 보통 (Soso) | 물 O + 빛 X | 1 | 물만 있음 |
| 🌞 햇빛 (Sunshine) | 물 X + 빛 O | 1 | 빛만 있음 |
| 😢 슬픔 (Sad) | 물 X + 빛 X | 0 | 물과 빛 모두 부족 |

### 센서 임계값
```cpp
bool isWaterDetected = water > 100;   // 수분 감지 기준
bool isBright = cds > 24;             // 조도 감지 기준 (0~100 스케일)
```

### 데이터 전송 주기
- 센서 측정: 2초마다
- 평균값 계산: 5회 측정 후 (10초마다)
- 블루투스 전송: 평균값 계산 후 자동 전송

### 센서 값 범위
- **조도(CDS)**: 0~100 (0=어두움, 100=밝음)
- **수분(Water)**: 0~1023 (높을수록 습함)
- **행복도(Happy)**: 0~2 (0=슬픔, 1=보통, 2=행복)

## 🔧 DEBUG

### OLED 화면이 안 켜지는 경우
- [ ] 5V, GND 연결 확인
- [ ] SPI 핀 연결 확인 (D8, D9, D10, D11, D13)
- [ ] OLED가 SPI 방식인지 확인 (I2C 방식은 코드 수정 필요)
- [ ] 시리얼 모니터에 "OLED 초기화 실패" 메시지 확인

### 센서 값이 이상한 경우
- [ ] 조도 센서: 10kΩ 저항 연결 확인
- [ ] 수분 센서: VCC, GND, AO 핀 연결 확인
- [ ] 시리얼 모니터로 값 확인 (코드 주석 해제)

```cpp
// 센서 값 확인용 코드 (주석 해제)
Serial.print("cds : ");
Serial.print(cds);
Serial.print(", water : ");
Serial.print(water);
Serial.print(", happyness : ");
Serial.println(happy);
```

### 업로드가 안 돼는 경우
```
Error: avrdude: stk500_recv(): programmer is not responding
```
- [ ] 블루투스 모듈 RX/TX 핀 분리
- [ ] USB 케이블 연결 확인
- [ ] 올바른 COM 포트 선택
- [ ] 다른 USB 포트 시도

### 컴파일 에러
```
Error: 'Adafruit_SSD1306' does not name a type
```
- [ ] 라이브러리 설치 확인
- [ ] Arduino IDE 재시작

## 🎯 커스터마이징

### 센서 임계값 조정
```cpp
// 수분 감지 민감도 조정
bool isWaterDetected = water > 100;  // 값을 높이면 덜 민감

// 조도 감지 민감도 조정
bool isBright = cds > 24;  // 값을 높이면 더 밝아야 감지
```

### 데이터 전송 주기 변경
```cpp
// 센서 측정 주기 (초)
int sensorTime = 2;  // 기본 2초

// 평균값 계산 횟수
if(cnt >= 5 * 1)  // 5회 → 원하는 횟수로 변경
```

---
