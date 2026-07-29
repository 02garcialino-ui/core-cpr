#include "simulador_falla.h"
#include "config.h"
#include "logger.h"

void simuladorFallaIniciar() {
  if (!MODO_SIMULACION) return;   // no existe en hardware real

  pinMode(PIN_SIM_FALLA_SENSORES, INPUT_PULLUP);
  logMsg(LOG_INFO, "SIM_FALLA", "Switch de prueba (falla de sensores) listo en GPIO " +
                                String(PIN_SIM_FALLA_SENSORES));
}

bool simuladorFallaActiva() {
  if (!MODO_SIMULACION) return false;

  return (digitalRead(PIN_SIM_FALLA_SENSORES) == LOW);
}
