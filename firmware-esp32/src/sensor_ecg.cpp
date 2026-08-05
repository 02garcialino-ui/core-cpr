#include "sensor_ecg.h"
#include "ads1115.h"
#include "config.h"
#include "logger.h"

void ecgIniciar() {
  if (MODO_SIMULACION) {
    pinMode(PIN_ECG_SIM, INPUT);
    logMsg(LOG_INFO, "ECG", "Modo SIMULADO: leyendo potenciometro en GPIO " +
                            String(PIN_ECG_SIM));
    // === reemplazar por sensor real cuando llegue ===
  } else {
    ads1115Iniciar();
    logMsg(LOG_INFO, "ECG", "Modo REAL: leyendo AD8232 via ADS1115");
  }
}

// ESTA es la unica funcion que cambia entre simulado y real.
// Todo lo demas del proyecto funciona igual en los dos casos.
float ecgLeerValor() {
  if (MODO_SIMULACION) {
    // El ADC de la ESP32 entrega 0..4095. Lo escalamos a 0..3300 mV
    // para que el numero se parezca a lo que dara el sensor real.
    int lectura = analogRead(PIN_ECG_SIM);
    return (lectura / 4095.0) * 3300.0;
    // === reemplazar por sensor real cuando llegue ===
  } else {
    return ads1115LeerCanal(ADS1115_CANAL_ECG);
  }
}

// La decision. Esta logica NO cambia al pasar a hardware real:
// solo cambia de donde salio el numero, no como se interpreta.
bool ecgHaySenal() {
  float valor = ecgLeerValor();

  // NAN significa que el sensor fallo. No es lo mismo que "no hay senal":
  // un paciente vivo con el sensor roto no debe tratarse como paro.
  // La FSM decidira que hacer con esto en la Fase 7 (T7.2).
  if (isnan(valor)) {
    logMsg(LOG_ERROR, "ECG", "Lectura invalida: el sensor no responde");
    return false;
  }

  bool haySenal = (valor > UMBRAL_ECG_MV);

  if (MODO_DEBUG) {
    logMsg(LOG_DEBUG, "ECG", "valor=" + String(valor, 1) + "mV" +
                             "  umbral=" + String(UMBRAL_ECG_MV) + "mV" +
                             "  -> haySenal=" + (haySenal ? "SI" : "NO"));
  }

  return haySenal;
}
