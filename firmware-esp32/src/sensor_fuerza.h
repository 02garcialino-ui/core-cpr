#ifndef SENSOR_FUERZA_H
#define SENSOR_FUERZA_H

#include <Arduino.h>

// =====================================================
//  Modulo de fuerza (celda de carga + HX711)
//
//  Responde una sola pregunta al resto del sistema:
//  "la fuerza aplicada, se paso del limite?"
//
//  A diferencia del ECG y el PPG, Wokwi SI simula el chip HX711 real.
//  Por eso este modulo no necesita una rama simulado/real: el mismo
//  codigo de lectura sirve para la simulacion y para el hardware fisico.
// =====================================================

// Prepara el sensor (tara incluida). Llamar una vez en setup().
void fuerzaIniciar();

// Devuelve la fuerza medida en Newtons.
float fuerzaLeerNewtons();

// La pregunta que le importa a la maquina de estados.
// true = se supero el limite de fuerza | false = dentro del limite
bool fuerzaExcedeLimite();

#endif
