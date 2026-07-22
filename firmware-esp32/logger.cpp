#include "logger.h"
#include "config.h"

// Traduce el nivel a texto. 'static' = privada, solo se usa dentro de este archivo.
static String nivelATexto(NivelLog nivel) {
  switch (nivel) {
    case LOG_INFO:  return "INFO";
    case LOG_DEBUG: return "DEBUG";
    case LOG_WARN:  return "WARN";
    case LOG_ERROR: return "ERROR";
    default:        return "?";
  }
}

// Arma el tiempo transcurrido desde el arranque, con formato mm:ss.mmm
// millis() devuelve los milisegundos que lleva encendida la ESP32.
static String marcaDeTiempo() {
  unsigned long ms = millis();
  unsigned long minutos      = ms / 60000;
  unsigned long segundos     = (ms / 1000) % 60;
  unsigned long milisegundos = ms % 1000;

  char buffer[16];
  sprintf(buffer, "%02lu:%02lu.%03lu", minutos, segundos, milisegundos);
  return String(buffer);
}

void loggerIniciar() {
  Serial.begin(SERIAL_BAUDIOS);
  delay(100);   // pequeña espera: da tiempo a que la consola quede lista
  logMsg(LOG_INFO, "LOGGER", "Sistema iniciado");
}

void logMsg(NivelLog nivel, const String& origen, const String& mensaje) {
  // Si MODO_DEBUG está apagado, los DEBUG no se imprimen.
  // Así se limpia la consola sin borrar ni una línea de código.
  if (nivel == LOG_DEBUG && !MODO_DEBUG) {
    return;
  }

  Serial.println("[" + marcaDeTiempo() + "]"
                 "[" + origen + "]"
                 "[" + nivelATexto(nivel) + "] " + mensaje);
}
