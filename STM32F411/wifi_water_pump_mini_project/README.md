# 🌱 Smart Flowerpot (STM32F4) — Water Pump, NeoPixel, Wi-Fi(ESP)

> 워터펌프 제어 + NeoPixel LED 표시 + ESP(AT) 기반 TCP 클라이언트 통신  
> 본 README는 `main.c`(아래 버전 기준)에서 실제로 사용된 **핀맵, 인터럽트 동작, 타이머/ DMA, UART 용도**를 정리했습니다.  

---

## 📁 프로젝트 개요

- **MCU**: STM32F411
- **주요 주변장치**
  - **Water Pump x2**: GPIO 출력 제어 (ON 유지 후 5초 타이머로 자동 OFF)
  - **NeoPixel (WS2812/WS2812B) x4**: **TIM2 PWM + DMA** 로 800 kHz 비트스트림 구현
  - **Wi-Fi (ESP8266/ESP-AT)**: **UART6 38,400 bps** – 서버 접속/프로토콜 송수신
  - **Debug 콘솔**: **UART2 115,200 bps**

- **프로토콜**
  - 수신: `[SENDER]LED@ON\n`, `[SENDER]LED@OFF\n`, `[SENDER]WATER@ON\n`
  - 송신: `"[SENDER]LED@ON\n"` 처럼 **동일 포맷**으로 ACK 회신

---

## 🔌 Pin Map (하드웨어 연결표)

| 기능 | 핀(매크로) | 방향 | 설명 |
|---|---|---|---|
| **Water Pump #1** | `PC0` | OUT | `HAL_GPIO_WritePin(GPIOC, water_pump_1_Pin, GPIO_PIN_SET)` |
| **Water Pump #2** | `PC1` | OUT | `HAL_GPIO_WritePin(GPIOC, water_pump_2_Pin, GPIO_PIN_RESET)`| 
| **NeoPixel DIN** | `TIM2_CH3` @ `PB10` | OUT | DMA로 800 kHz PWM 비트스트림 전송. |
| **ESP UART (Wi-Fi)** | `USART6` @ `PC7` @ `PC6` | TX/RX | 38,400 bps, AT 커맨드/데이터 송수신 |
| **Debug UART** | `USART2` @ `PA2` @ `PA3` | TX/RX | 115,200 bps, `printf()`/로그 |

---

---

## ⏱️ 타이머/인터럽트/ DMA — 설계 의도와 동작

### TIM2 (NeoPixel, PWM + DMA)
- **목적**: WS2812의 800 kHz 비트 타이밍을 정확히 생성  
- **설정**: `Prescaler=0`, `Period=104` → `84 MHz / (104+1) = 800 kHz`  
- **듀티 인코딩**:
  - `#define NEOPIXEL_ZERO 33`  → 33/105 ≈ **31%** High (약 0.4 µs)
  - `#define NEOPIXEL_ONE  67`  → 67/105 ≈ **64%** High (약 0.8 µs)  
  → WS2812 타이밍 스펙(`T0H≈0.4µs`, `T1H≈0.8µs`)을 만족
- **DMA 흐름**:
  1. `pixel[i].data`(24bit, GRB 순) 각 비트를 **`dmaBuffer`** 로 변환
  2. `HAL_TIM_PWM_Start_DMA()` 호출 시 DMA가 **OC 레지스터**에 듀티를 연속 전송
  3. 프레임 끝(마지막 0)에서 **`HAL_TIM_PWM_PulseFinishedCallback()`** 콜백 → `HAL_TIM_PWM_Stop_DMA()` 로 정지  
     *(리셋 타임(>50µs)은 송출 중지로 확보)*

### TIM3 (1 ms SysTick 역할)
- **목적**: **주기적 작업 스케줄링** (네트워크 상태 체크 등)
- **설정**: `Prescaler = 84-1`, `Period = 1000-1` → **1 ms** 주기
- **인터럽트**: `HAL_TIM_PeriodElapsedCallback()`  
  - 매 1 ms 누적 → 1000 ms 마다 `tim3Flag1Sec=1`, `tim3Sec++`
  - **10초마다** ESP 연결상태 확인 및 `esp_client_conn()` 재시도

### TIM4 (물주기 타이밍)
- **목적**: **펌프 동작 시간(5초)** 관리
- **설정**: `Prescaler = 84-1`, `Period = 1000-1` → **1 ms**
- **인터럽트**: `HAL_TIM_PeriodElapsedCallback()`  
  - `waterflag == 1` 인 동안 1 ms 카운팅
  - **5,000 ms** 경과 시 `waterpump_off_flag = 0` 로 전환 → 펌프 OFF 처리 및 ACK 송신

---

## 📡 UART 구성과 용도

| UART | Baud | 용도 | 코드 기준 사용처 |
|---|---:|---|---|
| **USART2** | **115,200** | **디버그 콘솔** (`printf`) 및 테스트 수신 | `PUTCHAR_PROTOTYPE`, `rx2Flag`, `rx2Data` 로 수신 출력 |
| **USART6** | **38,400** | **ESP-AT 모듈** 제어/데이터 송수신 | `drv_uart_init()`, `drv_esp_init()`, `esp_send_data()`, `esp_client_conn()` 등 |

> 정리: **UART 2개 사용** — `USART2(디버그)`, `USART6(ESP)`.

---

## 💧 물주기 FSM (WATER 명령 처리)

1. 서버 → MCU: **`[NAME]WATER@ON\n`**
2. `esp_event()`에서 `pArray` 파싱 → `waterflag = 1`
3. **즉시 ON**:  
   ```
   HAL_GPIO_WritePin(GPIOC, water_pump_1_Pin, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOC, water_pump_2_Pin, GPIO_PIN_RESET);
