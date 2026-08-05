#ifndef SENSOR_PROFUNDIDAD_H
#define SENSOR_PROFUNDIDAD_H

#include <Arduino.h>

// =====================================================
//  Prueba aislada de hardware real: sensor de profundidad VL53L0X.
//  Usa la libreria "VL53L0X" de Pololu (instalada por el usuario via
//  el Gestor de Librerias) -- a diferencia del ADS1115, este chip
//  necesita una secuencia de inicializacion demasiado compleja para
//  escribirla a mano de forma confiable.
// =====================================================

// Prepara el sensor. Llamar una vez en setup(). Comparte el mismo bus
// I2C que el ADS1115 (direccion distinta: 0x29 vs 0x48, no chocan).
void profundidadIniciar();

// Devuelve la distancia medida en centimetros. Devuelve NAN si el
// sensor no responde o si la medicion da timeout.
float profundidadLeerCm();

#endif
