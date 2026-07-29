#include "sensor_profundidad.h"
#include "config.h"
#include "logger.h"
#include "simulador_falla.h"

void profundidadIniciar() {
  if (MODO_SIMULACION) {
    pinMode(PIN_PROFUNDIDAD_SIM, INPUT);
    logMsg(LOG_INFO, "PROFUNDIDAD", "Modo SIMULADO: leyendo potenciometro en GPIO " +
                                    String(PIN_PROFUNDIDAD_SIM));
    // === reemplazar por sensor real cuando llegue ===
  } else {
    logMsg(LOG_ERROR, "PROFUNDIDAD", "Modo REAL: VL53L0X aun no implementado");
    // === reemplazar por sensor real cuando llegue ===
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
    return NAN;   // === reemplazar por sensor real cuando llegue (VL53L0X) ===
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
