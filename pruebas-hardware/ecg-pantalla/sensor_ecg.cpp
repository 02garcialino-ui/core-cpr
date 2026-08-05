#include "sensor_ecg.h"
#include "ads1115.h"
#include "config.h"
#include "logger.h"

// Copia sin cambios de firmware-esp32/sensor_ecg.cpp. Como en config.h
// de esta prueba MODO_SIMULACION siempre es false, aqui SIEMPRE se lee
// el AD8232 real a traves del ADS1115.

void ecgIniciar() {
  if (MODO_SIMULACION) {
    // No aplica en esta prueba (MODO_SIMULACION es siempre false aqui).
  } else {
    ads1115Iniciar();
    logMsg(LOG_INFO, "ECG", "Modo REAL: leyendo AD8232 via ADS1115");
  }
}

float ecgLeerValor() {
  if (MODO_SIMULACION) {
    return NAN; // no aplica en esta prueba
  } else {
    return ads1115LeerCanal(ADS1115_CANAL_ECG);
  }
}

bool ecgHaySenal() {
  float valor = ecgLeerValor();

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
