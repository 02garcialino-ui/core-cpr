#include "selector_modo.h"
#include "config.h"
#include "logger.h"

void selectorModoIniciar() {
  pinMode(PIN_SELECTOR_MODO, INPUT_PULLUP);
  logMsg(LOG_INFO, "MODO", "Selector de modo listo en GPIO " + String(PIN_SELECTOR_MODO));
}

ModoPaciente selectorModoLeer() {
  // Con INPUT_PULLUP, el pin da LOW cuando el switch conecta a GND.
  bool esNino = (digitalRead(PIN_SELECTOR_MODO) == LOW);
  return esNino ? MODO_NINO : MODO_ADULTO;
}
