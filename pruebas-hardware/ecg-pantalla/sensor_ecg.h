#ifndef SENSOR_ECG_H
#define SENSOR_ECG_H

#include <Arduino.h>

// Copia sin cambios de firmware-esp32/sensor_ecg.h.

void ecgIniciar();
float ecgLeerValor();
bool ecgHaySenal();

#endif
