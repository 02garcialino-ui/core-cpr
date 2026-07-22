// =====================================================
//  Core CPR - Firmware principal (ESP32)
//  Estado: Fase 1 - base del proyecto (config + logger)
// =====================================================

#include "config.h"
#include "logger.h"

void setup() {
  loggerIniciar();

  pinMode(PIN_LED_ESTADO, OUTPUT);
  logMsg(LOG_INFO, "MAIN", "LED de estado listo en GPIO " + String(PIN_LED_ESTADO));

  if (MODO_SIMULACION) {
    logMsg(LOG_WARN, "MAIN", "MODO_SIMULACION activo: los datos NO son de sensores reales");
  }
}

void loop() {
  digitalWrite(PIN_LED_ESTADO, HIGH);
  logMsg(LOG_DEBUG, "MAIN", "LED = ENCENDIDO");
  delay(500);

  digitalWrite(PIN_LED_ESTADO, LOW);
  logMsg(LOG_DEBUG, "MAIN", "LED = apagado");
  delay(500);
}
