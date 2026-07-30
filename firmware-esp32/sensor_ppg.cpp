#include "sensor_ppg.h"
#include "config.h"
#include "logger.h"
#include "simulador_falla.h"

void ppgIniciar() {
  if (MODO_SIMULACION) {
    pinMode(PIN_PPG_SIM, INPUT);
    logMsg(LOG_INFO, "PPG", "Modo SIMULADO: leyendo potenciometro en GPIO " +
                            String(PIN_PPG_SIM));
    // === reemplazar por sensor real cuando llegue ===
  } else {
    logMsg(LOG_ERROR, "PPG", "Modo REAL: MAX30102 aun no implementado");
    // === reemplazar por sensor real cuando llegue ===
  }
}

// ESTA es la unica funcion que cambia entre simulado y real.
// Todo lo demas del proyecto funciona igual en los dos casos.
float ppgLeerValor() {
  if (MODO_SIMULACION) {
    if (simuladorFallaActiva()) {
      return NAN;   // === SOLO PARA PRUEBAS (T7.2) ===
    }

    // El ADC de la ESP32 entrega 0..4095. Lo escalamos a 0..3300 mV
    // para que el numero se parezca a lo que dara el sensor real.
    int lectura = analogRead(PIN_PPG_SIM);
    return (lectura / 4095.0) * 3300.0;
    // === reemplazar por sensor real cuando llegue ===
  } else {
    return NAN;   // === reemplazar por sensor real cuando llegue (MAX30102) ===
  }
}

// La decision. Esta logica NO cambia al pasar a hardware real:
// solo cambia de donde salio el numero, no como se interpreta.
bool ppgHayPulso() {
  float valor = ppgLeerValor();

  // NAN significa que el sensor fallo. No es lo mismo que "no hay pulso":
  // un paciente con pulso pero con el sensor roto no debe tratarse como paro.
  // Aqui sigue devolviendo false (tratado como "sin pulso"): quien decide
  // si hay que preguntar antes de asumir el paro es la FSM, usando
  // ppgSensorFallo() para distinguir el caso (T7.2 Parte B).
  if (isnan(valor)) {
    logMsg(LOG_ERROR, "PPG", "Lectura invalida: el sensor no responde");
    return false;
  }

  bool hayPulso = (valor > UMBRAL_PPG_MV);

  if (MODO_DEBUG) {
    logMsg(LOG_DEBUG, "PPG", "valor=" + String(valor, 1) + "mV" +
                             "  umbral=" + String(UMBRAL_PPG_MV) + "mV" +
                             "  -> hayPulso=" + (hayPulso ? "SI" : "NO"));
  }

  return hayPulso;
}

bool ppgSensorFallo() {
  return isnan(ppgLeerValor());
}
