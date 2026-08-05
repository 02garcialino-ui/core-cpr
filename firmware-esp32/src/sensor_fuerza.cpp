#include "sensor_fuerza.h"
#include "config.h"
#include "logger.h"
#include "simulador_falla.h"
#include <HX711.h>

HX711 balanza;

void fuerzaIniciar() {
  balanza.begin(PIN_HX711_DT, PIN_HX711_SCK);
  balanza.set_scale(HX711_CALIBRACION);
  balanza.tare();   // el "cero" se toma sin peso encima
  logMsg(LOG_INFO, "FUERZA", "HX711 listo (DT=" + String(PIN_HX711_DT) +
                             " SCK=" + String(PIN_HX711_SCK) + ")");
}

float fuerzaLeerNewtons() {
  if (simuladorFallaActiva()) {
    return NAN;   // === SOLO PARA PRUEBAS (T7.2) ===
  }

  float kg = balanza.get_units(10);   // promedio de 10 lecturas, mas estable
  return kg * 9.81;                   // F = m * g
}

// La decision. Esta logica NO cambia al pasar a hardware real: solo
// cambia de donde salio el numero, no como se interpreta.
bool fuerzaExcedeLimite() {
  float newtons = fuerzaLeerNewtons();

  // NAN significa que el sensor fallo. Se trata IGUAL que exceder el
  // limite (T7.2): no se deja el motor operando a ciegas sin saber si
  // esta pasando el limite de verdad, mismo criterio que el paro de
  // emergencia.
  if (isnan(newtons)) {
    logMsg(LOG_ERROR, "FUERZA", "Lectura invalida: el sensor no responde");
    return true;
  }

  bool excede = (newtons > FUERZA_MAX_N);

  if (MODO_DEBUG) {
    logMsg(LOG_DEBUG, "FUERZA", "valor=" + String(newtons, 1) + "N" +
                                "  limite=" + String(FUERZA_MAX_N) + "N" +
                                "  -> excede=" + (excede ? "SI" : "NO"));
  }

  if (excede) {
    logMsg(LOG_WARN, "FUERZA", "LIMITE DE FUERZA EXCEDIDO");
  }

  return excede;
}
