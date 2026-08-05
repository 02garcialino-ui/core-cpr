#include "sensor_profundidad.h"
#include "config.h"
#include "logger.h"
#include "simulador_falla.h"

// El modo real necesita la libreria "Adafruit VL53L0X" (Arduino IDE:
// Sketch > Include Library > Manage Libraries > buscar "Adafruit VL53L0X").
#if !MODO_SIMULACION
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

static Adafruit_VL53L0X g_lox = Adafruit_VL53L0X();
static bool g_profundidadOk = false;   // false hasta que begin() confirme el chip
#endif

void profundidadIniciar() {
  if (MODO_SIMULACION) {
    pinMode(PIN_PROFUNDIDAD_SIM, INPUT);
    logMsg(LOG_INFO, "PROFUNDIDAD", "Modo SIMULADO: leyendo potenciometro en GPIO " +
                                    String(PIN_PROFUNDIDAD_SIM));
    // === reemplazar por sensor real cuando llegue ===
  } else {
#if !MODO_SIMULACION
    Wire.begin();   // SDA=21, SCL=22 (idempotente si ya lo llamo el ECG/PPG)
    g_profundidadOk = g_lox.begin();

    if (g_profundidadOk) {
      logMsg(LOG_INFO, "PROFUNDIDAD", "Modo REAL: VL53L0X detectado");
    } else {
      logMsg(LOG_ERROR, "PROFUNDIDAD", "Modo REAL: VL53L0X no responde (revisar cableado I2C)");
    }
#endif
  }
}

// ESTA es la unica funcion que cambia entre simulado y real.
// Todo lo demas del proyecto funciona igual en los dos casos.
float profundidadLeerCm() {
  if (MODO_SIMULACION) {
    if (simuladorFallaActiva()) {
      return NAN;   // === SOLO PARA PRUEBAS (T7.2) ===
    }

    // El ADC de la ESP32 entrega 0..4095. Lo escalamos al rango simulado
    // (0-10 cm) para poder probar bien el limite de 5 cm.
    int lectura = analogRead(PIN_PROFUNDIDAD_SIM);
    return (lectura / 4095.0) * PROFUNDIDAD_SIM_MAX_CM;
    // === reemplazar por sensor real cuando llegue ===
  } else {
#if !MODO_SIMULACION
    if (!g_profundidadOk) {
      return NAN;   // el chip nunca respondio en profundidadIniciar()
    }

    VL53L0X_RangingMeasurementData_t medicion;
    g_lox.rangingTest(&medicion, false);

    // RangeStatus 4 = "fuera de rango" / sin nada enfrente que refleje
    // bien la señal -- no es una falla del chip, es una lectura valida
    // de "no hay nada ahi". La tratamos igual que un fallo (NAN) porque,
    // para el limite de seguridad, no saber la profundidad real debe
    // frenar el motor igual que si el sensor estuviera roto (mismo
    // criterio que el resto del proyecto). OJO: PROVISIONAL -- se afina
    // con el sensor real en mano si hace falta distinguir mejor los
    // otros codigos de estado (0-3).
    if (medicion.RangeStatus == 4) {
      return NAN;
    }

    return medicion.RangeMilliMeter / 10.0;   // mm -> cm
#else
    return NAN;
#endif
  }
}

// La decision. Esta logica NO cambia al pasar a hardware real:
// solo cambia de donde salio el numero, no como se interpreta.
bool profundidadExcedeLimite() {
  float valor = profundidadLeerCm();

  // NAN significa que el sensor fallo. Se trata IGUAL que exceder el
  // limite (T7.2): no se deja el motor operando a ciegas sin saber si
  // esta pasando el limite de verdad, mismo criterio que el paro de
  // emergencia.
  if (isnan(valor)) {
    logMsg(LOG_ERROR, "PROFUNDIDAD", "Lectura invalida: el sensor no responde");
    return true;
  }

  bool excede = (valor > PROFUNDIDAD_MAX_CM);

  if (MODO_DEBUG) {
    logMsg(LOG_DEBUG, "PROFUNDIDAD", "valor=" + String(valor, 1) + "cm" +
                                     "  limite=" + String(PROFUNDIDAD_MAX_CM, 1) + "cm" +
                                     "  -> excede=" + (excede ? "SI" : "NO"));
  }

  if (excede) {
    logMsg(LOG_WARN, "PROFUNDIDAD", "LIMITE DE PROFUNDIDAD EXCEDIDO");
  }

  return excede;
}
