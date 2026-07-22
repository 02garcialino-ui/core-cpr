# CONVENCIONES DE CÓDIGO — Core CPR

> Cómo está organizado y escrito el código del proyecto. Sirve para que todo sea **uniforme, fácil de depurar y con una transición transparente de la simulación al hardware real**.

---

## 1. Estructura de carpetas y archivos

```
Core-CPR/
│
├── firmware-esp32/               ← El "cerebro" (corre en la ESP32)
│   ├── firmware-esp32.ino         ← Principal: setup() y loop()
│   ├── config.h                   ← Pines, umbrales, MODO_SIMULACION, MODO_DEBUG
│   ├── logger.h / logger.cpp      ← Los logs (tiempo + origen + valores)
│   │
│   │   ── Sensores de DIAGNÓSTICO ──
│   ├── sensor_ecg.h / .cpp        ← Lógica del ECG (usa el ADS1115)
│   ├── ads1115.h / .cpp           ← Driver del ADC externo (separado, por uniformidad)
│   ├── sensor_ppg.h / .cpp        ← PPG (MAX30102)
│   │
│   │   ── Sensores de CONTROL/SEGURIDAD ──
│   ├── sensor_fuerza.h / .cpp     ← Célula de carga + HX711
│   ├── sensor_profundidad.h / .cpp← Láser VL53L0X
│   │
│   │   ── Actuación e interfaz ──
│   ├── actuador_motor.h / .cpp    ← NEMA 23 + homing
│   ├── hmi_pantalla.h / .cpp      ← Pantalla Nextion
│   ├── seguridad.h / .cpp         ← Paro, límites, buzzer
│   └── logica_fsm.h / .cpp        ← Máquina de estados / decisiones
│
├── simulador-ecg-arduino/        ← El Arduino aparte que "finge el corazón"
│   └── simulador-ecg-arduino.ino
│
├── docs/                         ← Documentación (este archivo, el plan, el BOM, etc.)
│   └── diagramas/
│
└── README.md
```

**Regla de módulos:** un archivo (`.h/.cpp`) por cada parte. Si algo falla, se toca solo ese módulo sin romper lo demás.

---

## 2. Nombres y formato

| Elemento | Estilo | Ejemplo |
|---|---|---|
| Archivos | minúsculas con guión bajo | `sensor_ecg.cpp` |
| Constantes y `#define` | MAYÚSCULAS con guión bajo | `PIN_ECG`, `FUERZA_MAX_N` |
| Funciones | prefijo del módulo + camelCase | `ecgHaySenal()` |
| Variables | camelCase | `valorActual` |
| Idioma | español | `haySenal`, `profundidad` |
| Comentarios | explican el "por qué", no solo el "qué" | — |

---

## 3. Las 3 convenciones clave

### a) Doble capa (lo que hace la transición transparente)
Cada módulo de sensor ofrece funciones simples (ej. `ecgHaySenal()`). **Por dentro** decide si lee el sensor real o un dato simulado, según `MODO_SIMULACION`. La lógica **nunca se entera** de cuál es cuál.

### b) Interruptores globales + marcas de "SIMULADO"
En `config.h`:
- `MODO_SIMULACION` → `true` = datos simulados | `false` = sensores reales.
- `MODO_DEBUG` → `true` = imprime logs detallados.

Cada trozo simulado lleva un comentario visible:
```cpp
// === reemplazar por sensor real cuando llegue ===
```

### c) Logs con valores y umbrales
`logMsg(nivel, origen, mensaje)` con niveles `LOG_INFO / LOG_DEBUG / LOG_WARN / LOG_ERROR`.
Se ve así en la consola:
```
[00:01.240][ECG][DEBUG] valor=0.02mV  umbral=100mV  -> haySenal=NO
```
Registramos: cambios de estado, lecturas, cálculos, decisiones, límites y selección de modo.

---

## 4. Ejemplo de referencia

**`config.h`**
```cpp
#ifndef CONFIG_H
#define CONFIG_H

// ---- Interruptores globales ----
#define MODO_SIMULACION  true    // true = datos simulados | false = sensores reales
#define MODO_DEBUG       true    // true = imprime logs detallados

// ---- Pines (ESP32) ----
#define PIN_ECG_LO_MAS    34
#define PIN_ECG_LO_MENOS  35
#define PIN_ECG_SIM       32     // entrada simulada (potenciómetro)

// ---- Umbrales clínicos (AHA - modo niño) ----
#define UMBRAL_ECG_MV      100   // por debajo = "sin señal" (mV)
#define PROFUNDIDAD_MAX_CM  5.0  // límite de profundidad (cm)
#define FUERZA_MAX_N        250  // límite de fuerza (N)
#define RITMO_CPM           110  // compresiones por minuto objetivo

#endif
```

**`sensor_ecg.cpp` (fragmento — muestra la doble capa)**
```cpp
// Lectura del valor: ESTA es la capa que cambia entre real y simulado
float ecgLeerValor() {
  if (MODO_SIMULACION) {
    int lectura = analogRead(PIN_ECG_SIM);
    return (lectura / 4095.0) * 330.0;   // escalado a "mV"
  } else {
    return ads1115LeerCanal(0);          // === reemplazar por sensor real ===
  }
}

// ¿Hay señal? Esta lógica NO cambia entre real y simulado
bool ecgHaySenal() {
  if (ecgElectrodoSuelto()) {
    logMsg(LOG_WARN, "ECG", "Electrodo desprendido -> lectura no confiable");
    return false;   // la FSM lo trata como FALLO, no como paro
  }
  float valor = ecgLeerValor();
  bool haySenal = (valor > UMBRAL_ECG_MV);
  if (MODO_DEBUG) {
    logMsg(LOG_DEBUG, "ECG", "valor=" + String(valor) +
           "mV umbral=" + String(UMBRAL_ECG_MV) +
           "mV -> haySenal=" + (haySenal ? "SI" : "NO"));
  }
  return haySenal;
}
```

---

## 5. Idea futura (no ahora)
Cuando tengamos 1–2 módulos probados a mano, evaluar crear un **skill "nuevo-módulo"** que genere `sensor_X.h/.cpp` ya con la doble capa, los logs y el `config.h` enganchado.
