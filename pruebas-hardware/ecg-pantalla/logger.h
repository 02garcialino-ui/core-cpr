#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

// Copia sin cambios de firmware-esp32/logger.h, para esta prueba aislada.

enum NivelLog {
  LOG_INFO,
  LOG_DEBUG,
  LOG_WARN,
  LOG_ERROR
};

void loggerIniciar();
void logMsg(NivelLog nivel, const String& origen, const String& mensaje);

#endif
