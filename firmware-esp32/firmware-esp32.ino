// =====================================================
//  Core CPR - Firmware principal (ESP32)
//  Estado: Fase 2 - T2.1 sensor de ECG (simulado)
// =====================================================

#include "config.h"
#include "logger.h"
#include "sensor_ecg.h"

// Guardamos el estado anterior para avisar SOLO cuando cambia.
// Si no, la consola se llena de mensajes repetidos y no se ve nada.
bool habiaSenal = false;
bool primeraVuelta = true;

void setup() {
  loggerIniciar();

  pinMode(PIN_LED_ESTADO, OUTPUT);
  logMsg(LOG_INFO, "MAIN", "LED de estado listo en GPIO " + String(PIN_LED_ESTADO));

  if (MODO_SIMULACION) {
    logMsg(LOG_WARN, "MAIN", "MODO_SIMULACION activo: los datos NO son de sensores reales");
  }

  ecgIniciar();
  logMsg(LOG_INFO, "MAIN", "Gira el potenciometro para simular latido / paro");
}

void loop() {
  bool haySenal = ecgHaySenal();

  // El LED es la senal visual: encendido = hay latido.
  digitalWrite(PIN_LED_ESTADO, haySenal ? HIGH : LOW);

  if (primeraVuelta || haySenal != habiaSenal) {
    if (haySenal) {
      logMsg(LOG_INFO, "MAIN", ">>> HAY SENAL CARDIACA (paciente con actividad)");
    } else {
      logMsg(LOG_WARN, "MAIN", ">>> SIN SENAL CARDIACA (posible paro)");
    }
    habiaSenal = haySenal;
    primeraVuelta = false;
  }

  delay(500);   // media lectura por segundo alcanza para ver que pasa
}
