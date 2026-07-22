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

// ---- Límites clínicos (AHA - modo niño) ----
// Fuente: estándar AHA pediátrico, según se definió en T0.1.
#define PROFUNDIDAD_MAX_CM   5.0   // profundidad máxima de compresión (cm)
#define FUERZA_MAX_N         250   // fuerza máxima admitida (N)
#define RITMO_CPM_MIN        100   // ritmo mínimo (compresiones por minuto)
#define RITMO_CPM_MAX        120   // ritmo máximo (compresiones por minuto)

#endif
