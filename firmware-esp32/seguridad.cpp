#include "seguridad.h"
#include "config.h"
#include "logger.h"

void seguridadIniciar() {
  pinMode(PIN_PARO_EMERGENCIA, INPUT_PULLUP);
  logMsg(LOG_INFO, "SEGURIDAD", "Paro de emergencia listo en GPIO " + String(PIN_PARO_EMERGENCIA));
}

bool paroEmergenciaActivo() {
  // NC: circuito cerrado (LOW) = normal. Circuito abierto (HIGH) = parar
  // (presionado o cable cortado -- mismo efecto, a proposito).
  return (digitalRead(PIN_PARO_EMERGENCIA) == HIGH);
}
