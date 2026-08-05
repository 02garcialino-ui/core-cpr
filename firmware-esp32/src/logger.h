#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

// Niveles de gravedad de un mensaje.
// Permiten filtrar: en producción se apagan los DEBUG y quedan los importantes.
enum NivelLog {
  LOG_INFO,    // marcha normal ("modo niño seleccionado")
  LOG_DEBUG,   // detalle técnico ("valor=0.02mV umbral=100mV")
  LOG_WARN,    // algo raro pero no grave ("electrodo flojo")
  LOG_ERROR    // fallo serio ("fuerza excedida, abortando")
};

// Arranca la consola serial. Llamar una vez en setup().
void loggerIniciar();

// Imprime un mensaje con formato uniforme:
//   [mm:ss.mmm][ORIGEN][NIVEL] mensaje
void logMsg(NivelLog nivel, const String& origen, const String& mensaje);

#endif
