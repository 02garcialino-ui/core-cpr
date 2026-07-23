#ifndef CONFIG_H
#define CONFIG_H

// =====================================================
//  Core CPR - Configuración central
//  Todos los números del proyecto viven aquí.
//  Regla: si es un valor ajustable, va en este archivo.
// =====================================================

// ---- Interruptores globales ----
#define MODO_SIMULACION  true    // true = datos simulados | false = sensores reales
#define MODO_DEBUG       true    // true = imprime logs de nivel DEBUG

// ---- Comunicación ----
#define SERIAL_BAUDIOS   115200  // velocidad de la consola serial

// ---- Pines (ESP32 DevKitC v4) ----
// OJO: usar los pines rotulados con el número solo (2, 4, 5...).
// Los pines CLK, CMD, D0-D3 son de la memoria flash: NO se usan.
#define PIN_LED_ESTADO   2       // LED de prueba / indicador de actividad

// ---- ECG (T2.1) ----
// GPIO 34 es de SOLO ENTRADA y tiene ADC. Ideal para un potenciometro.
// En simulacion aqui va el potenciometro; con hardware real se usa el ADS1115.
#define PIN_ECG_SIM      34      // entrada simulada del ECG (potenciometro)
#define ADS1115_DIR_I2C  0x48    // direccion I2C del ADS1115 (ADDR a GND)
#define ADS1115_CANAL_ECG 0      // canal del ADS1115 donde entra el AD8232

// ---- Umbrales del ECG ----
// Por debajo de este valor consideramos que NO hay senal cardiaca.
// OJO: este numero es PROVISIONAL. Se recalibra con el sensor real (ver T9.4).
// 1500 = punto medio del rango del potenciometro (0-3300mV simulado).
#define UMBRAL_ECG_MV    1500    // milivolts

// ---- PPG (T2.2) ----
// GPIO 35 es de SOLO ENTRADA y tiene ADC, igual que el 34 del ECG.
// El sensor real (MAX30102) no esta disponible en Wokwi, asi que en
// simulacion tambien usamos un potenciometro (mismo patron que el ECG).
#define PIN_PPG_SIM      35      // entrada simulada del PPG (potenciometro)
#define PIN_LED_PPG      4       // LED que indica "hay pulso"

// ---- Umbral del PPG ----
// Por debajo de este valor consideramos que NO hay pulso.
// OJO: este numero es PROVISIONAL. Se recalibra con el sensor real (ver T9.4).
// 1650 = punto medio del rango del potenciometro (0-3300mV simulado).
#define UMBRAL_PPG_MV    1650    // milivolts

// ---- Fuerza (T2.3) ----
// El HX711 SI esta simulado en Wokwi (chip real, no un potenciometro),
// asi que el mismo codigo de lectura sirve para simulacion y para hardware real.
#define PIN_HX711_DT     16      // pin de datos (DOUT) del HX711
#define PIN_HX711_SCK    17      // pin de reloj (SCK) del HX711
#define PIN_LED_FUERZA   5       // LED que indica "fuerza excede el limite"

// Conteos crudos por kilogramo, segun el rango del chip simulado tipo "50kg"
// (0-21000 conteos = 0-50 kg). OJO: PROVISIONAL, se recalibra con el sensor
// real (ver T9.4).
#define HX711_CALIBRACION 420.0

// ---- Profundidad (T2.4) ----
// GPIO 32 tiene ADC y puede usarse como entrada, igual que el 34 y el 35.
// El sensor real (VL53L0X) no esta disponible en Wokwi, asi que en
// simulacion tambien usamos un potenciometro (mismo patron que ECG/PPG).
#define PIN_PROFUNDIDAD_SIM   32   // entrada simulada de profundidad (potenciometro)
#define PIN_LED_PROFUNDIDAD   18  // LED que indica "profundidad excede el limite"

// Rango simulado del potenciometro: 0-10 cm (no el rango real del VL53L0X,
// que es mucho mayor). Se elige asi para tener buena resolucion alrededor
// del limite de 5 cm. OJO: PROVISIONAL, se recalibra con el sensor real (T9.4).
#define PROFUNDIDAD_SIM_MAX_CM 10.0

// ---- Límites clínicos (AHA - modo niño) ----
// Fuente: estándar AHA pediátrico, según se definió en T0.1.
#define PROFUNDIDAD_MAX_CM   5.0   // profundidad máxima de compresión (cm)
#define FUERZA_MAX_N         250   // fuerza máxima admitida (N)
#define RITMO_CPM_MIN        100   // ritmo mínimo (compresiones por minuto)
#define RITMO_CPM_MAX        120   // ritmo máximo (compresiones por minuto)

#endif
