#ifndef SENSOR_PROFUNDIDAD_H
#define SENSOR_PROFUNDIDAD_H

#include <Arduino.h>

// =====================================================
//  Modulo de profundidad (laser VL53L0X)
//
//  Responde una sola pregunta al resto del sistema:
//  "la profundidad de la compresion, se paso del limite?"
//
//  Quien lo llama NO sabe ni le importa si el dato viene de un
//  potenciometro simulado o del VL53L0X real. Eso lo decide
//  MODO_SIMULACION dentro de este archivo (ver CONVENCIONES-CODIGO.md).
// =====================================================

// Prepara el sensor. Llamar una vez en setup().
void profundidadIniciar();

// Devuelve la profundidad medida en centimetros.
float profundidadLeerCm();

// La pregunta que le importa a la maquina de estados.
// true = se supero el limite de profundidad | false = dentro del limite
bool profundidadExcedeLimite();

#endif
