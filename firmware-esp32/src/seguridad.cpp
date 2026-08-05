#include "seguridad.h"
#include "config.h"
#include "logger.h"

void seguridadIniciar() {
  pinMode(PIN_PARO_EMERGENCIA, INPUT_PULLUP);
  logMsg(LOG_INFO, "SEGURIDAD", "Paro de emergencia listo en GPIO " + String(PIN_PARO_EMERGENCIA));

  pinMode(PIN_BOTON_REINTENTAR, INPUT_PULLUP);
  logMsg(LOG_INFO, "SEGURIDAD", "Boton de reintentar lectura listo en GPIO " + String(PIN_BOTON_REINTENTAR));
}

bool paroEmergenciaActivo() {
  // NC: circuito cerrado (LOW) = normal. Circuito abierto (HIGH) = parar
  // (presionado o cable cortado -- mismo efecto, a proposito).
  return (digitalRead(PIN_PARO_EMERGENCIA) == HIGH);
}

// ---- Boton "Reintentar lectura" (T6.4) ----
// Antirebote clasico: solo se confia en un cambio de estado despues de
// que la lectura se mantiene igual por DEBOUNCE_BOTON_MS. Devuelve true
// solo en el instante en que se confirma el flanco de bajada (recien
// presionado), no mientras se mantiene apretado.
static int g_botonReintentarUltimaLectura = HIGH;
static int g_botonReintentarUltimoEstable = HIGH;
static unsigned long g_botonReintentarUltimoCambioMs = 0;

bool botonReintentarPresionado() {
  int lectura = digitalRead(PIN_BOTON_REINTENTAR);

  if (lectura != g_botonReintentarUltimaLectura) {
    g_botonReintentarUltimoCambioMs = millis();
  }
  g_botonReintentarUltimaLectura = lectura;

  bool presionado = false;
  if ((millis() - g_botonReintentarUltimoCambioMs) > DEBOUNCE_BOTON_MS) {
    if (lectura == LOW && g_botonReintentarUltimoEstable == HIGH) {
      presionado = true;   // flanco de bajada, ya estable (no rebote)
    }
    g_botonReintentarUltimoEstable = lectura;
  }

  return presionado;
}
