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

// ---- Límites clínicos (AHA - modo niño) ----
// Fuente: estándar AHA pediátrico, según se definió en T0.1.
#define PROFUNDIDAD_MAX_CM   5.0   // profundidad máxima de compresión (cm)
#define FUERZA_MAX_N         250   // fuerza máxima admitida (N)
#define RITMO_CPM_MIN        100   // ritmo mínimo (compresiones por minuto)
#define RITMO_CPM_MAX        120   // ritmo máximo (compresiones por minuto)

#endif
