# 🌱 PLANT-SAN (プラントさん)

![Arduino](https://img.shields.io/badge/Arduino-00979D?style=flat&logo=Arduino&logoColor=white)
![STM32](https://img.shields.io/badge/STM32-03234B?style=flat&logo=STMicroelectronics&logoColor=white)
![Raspberry Pi](https://img.shields.io/badge/Raspberry%20Pi-A22846?style=flat&logo=Raspberry%20Pi&logoColor=white)
![MySQL](https://img.shields.io/badge/MySQL-4479A1?style=flat&logo=MySQL&logoColor=white)


반려동물처럼 교감하는 스마트 화분 IoT 시스템

![KakaoTalk_20250608_161509805_01](https://github.com/user-attachments/assets/194e8f7b-bcc1-46dd-8181-1b0c477eaec6)


---

## 🌟 프로젝트 소개

PLANT-SAN은 IoT 기술을 활용하여 식물과 사용자 간의 정서적 교감을 제공하는 스마트 화분 시스템입니다. 
반려동물을 키우는 것처럼 식물을 돌보는 과정에서 성취감과 힐링을 얻을 수 있으며, 필요시 원격으로도 식물을 관리할 수 있습니다.

**PLANT-SAN의 의미**: "プラントさん" - 일본어에서 사람에게 붙이는 존칭 '~씨'를 식물에게 사용하여, 식물을 반려 존재로 존중한다는 컨셉을 담았습니다.

---

## 💡 개발 배경

### 사회적 문제 인식

- **1인 가구 증가**: 대한민국 전체 가구의 약 30% 차지
- **독거노인 증가**: 10가구 중 1가구 수준으로 급증
- **정서적 고립**: 외로움, 우울감, 정신적 스트레스 증가

### 해결 방안

PLANT-SAN은 반려동물의 정서적 위안 효과는 유지하되, 실제 동물을 돌보는 부담(산책, 배변 처리, 의료비 등)은 없는 대안적 솔루션을 제공합니다.

---

## 🏗️ 시스템 구성


![인텔_프로젝트_구상도 (4)](https://github.com/user-attachments/assets/ca46d74b-1fd6-4987-b724-ac8ae157a014)



---

## ✨ 주요 기능

### 1. 실시간 모니터링 📊

- **센서 기반 상태 감지**
  - 조도(빛): 0~100 단계
  - 수분(물): 0~1023 단계
  - 행복도: 0~2 단계

- **표정 피드백 (OLED)**
  - 😊 행복 (Happy): 물 O + 빛 O
  - 😐 보통 (Soso): 물 O + 빛 X
  - 🌞 햇빛 (Sunshine): 물 X + 빛 O
  - 😢 슬픔 (Sad): 물 X + 빛 X
 
<img width="204" height="204" alt="image (1)" src="https://github.com/user-attachments/assets/c2219bc5-083a-425b-806c-5baa287ef359" />  <img width="204" height="207" alt="image" src="https://github.com/user-attachments/assets/319befe1-4cab-4af9-900b-d57b5105b674" />  <img width="189" height="204" alt="image (3)" src="https://github.com/user-attachments/assets/1647246a-ec3f-4090-b7e1-e191cea84d0c" />  <img width="189" height="204" alt="image (2)" src="https://github.com/user-attachments/assets/d2b8ed5d-dfda-46ff-934d-84ff30b4b2cc" />



### 2. 원격 제어 🌐

- **워터펌프 제어**
  - 모바일/웹에서 급수 명령 전송
  - 5초간 자동 급수 후 정지

- **NeoPixel LED 제어**
  - ON/OFF 원격 제어
  - 4개 LED 동시 제어

### 3. 데이터 관리 📈

- **MySQL 데이터베이스 저장**
  - 시간별 센서 데이터 기록
  - 행복도 추이 분석

- **PHP 웹 대시보드**
  - Chart.js 기반 실시간 그래프
  - CDS, Water, Happyness 데이터 시각화
  - 최신 상태 요약 표시

### 4. 자동 관리 모드 ⚙️

- **5일 이상 방치 시 자동 작동** (부가 기능)
  - 자동 급수
  - 자동 조명 제공
  - *주의: 핵심 기능은 사용자 참여 유도*

---

## **시연 영상**

**Click Me->**

https://youtube.com/shorts/rAXJnOaaPnQ?feature=share

---
## 🔧 기술 스택

### 하드웨어
- **Arduino Uno**: 센서 데이터 수집 및 OLED 제어
- **STM32F4**: 액추에이터 제어 및 WiFi 통신
- **Raspberry Pi 3/4**: IoT 서버 및 데이터베이스
- **HC-06**: 블루투스 통신 모듈
- **ESP8266**: WiFi 통신 모듈

### 소프트웨어
- **Arduino IDE**: Arduino 펌웨어 개발
- **STM32CubeIDE**: STM32 펌웨어 개발
- **C/C++**: 임베디드 시스템 프로그래밍
- **MySQL**: 센서 데이터 저장
- **PHP**: 웹 서버 및 API
- **Chart.js**: 데이터 시각화
- **HTML/CSS/JavaScript**: 웹 인터페이스

### 통신 프로토콜
- **Bluetooth (RFCOMM)**: Arduino ↔ Raspberry Pi
- **WiFi (TCP/IP)**: STM32 ↔ Raspberry Pi
- **Socket Programming**: 서버-클라이언트 통신

---


## 데이터 프로토콜

**Arduino → Server**
```
[SWJ_SQL]SENSOR@45@678@1
         └─┬─┘ └┬┘└┬┘└┬┘
           │    │  │  └─ 행복도 (0~2)
           │    │  └──── 수분값 (0~1023)
           │    └─────── 조도값 (0~100)
           └──────────── 송신 ID
```

**Server → STM32**
```
[SWJ_STM]WATER       # 급수
[SWJ_STM]LED@ON      # LED 켜기
[SWJ_STM]LED@OFF     # LED 끄기
```

---

## 핵심 가치

### 1. 유연한 자동화 레벨
- 기본: 사용자 참여 유도 (수동 관리)
- 옵션: 원격 제어 가능
- 비상: 5일 이상 방치 시 자동 관리

### 2. 데이터 기반 관리
- 실시간 센서 데이터 수집
- 데이터베이스 저장 및 분석
- 시각화를 통한 직관적 확인

### 3. 멀티플랫폼 지원
- PC 웹 인터페이스
- 모바일 웹 접근
- 다양한 환경에서 원격 제어

### 4. 확장 가능한 모듈 구조
- 어떤 화분에도 장착 가능
- 다양한 식물 지원
- 센서 추가 확장 용이

---

## 기존 제품과의 차별점

### vs. Ivy 스마트 플랜터 (10만원대)

| 구분 | Ivy 스마트 플랜터 | PLANT-SAN |
|------|-------------------|-----------|
| **피드백** | OLED 표정 | OLED 표정 |
| **원격 제어** | ❌ 없음 | ✅ 급수/조명 제어 |
| **크기** | 소형 고정 | 모듈형 (확장 가능) |
| **식물 종류** | 제한적 | 다양 (어떤 화분이나 장착) |
| **자동화** | 수동 관리만 | 유연한 관리 (수동/원격/자동) |
| **데이터** | 저장 없음 | MySQL 저장 및 시각화 |

### vs. 스마트팜

| 구분 | 스마트팜 | PLANT-SAN |
|------|----------|-----------|
| **목적** | 상업적 대량 재배 | 개인 정서적 교감 |
| **타겟** | B2B | B2C |
| **자동화** | 완전 자동 | 선택적 자동 (참여 유도) |
| **규모** | 대규모 | 소규모 (1~2개체) |
| **정서적 요소** | ❌ 없음 | ✅ OLED 표정, 피드백 |
| **가격** | 수백만원 ~ | 10만원 내외 예상 |



---

## 프로젝트 갤러리

### 기능 구성

![기능 구상도](https://github.com/user-attachments/assets/7daaa74e-9cb8-4af8-a2d3-44a0e24cdb7c)



### 웹 대시보드

<img width="1920" height="912" alt="image (4)" src="https://github.com/user-attachments/assets/36c6898d-c882-4ff6-a9e6-101d3bd3c6ba" />

<img width="833" height="526" alt="{B10D3B42-7AE9-4CFC-BA87-E6C82CE2283C}" src="https://github.com/user-attachments/assets/a5d47efb-388a-48ac-913b-39dcafb51d7d" />


---
