#ifndef SENSOR_ECG_H
#define SENSOR_ECG_H

#include <Arduino.h>

// =====================================================
//  Modulo del ECG (electrocardiograma)
//
//  Responde una sola pregunta al resto del sistema:
//  "hay actividad electrica del corazon, si o no?"
//
//  Quien lo llama NO sabe ni le importa si el dato viene de un
//  potenciometro simulado o del AD8232 real. Eso lo decide
//  MODO_SIMULACION dentro de este archivo (ver CONVENCIONES-CODIGO.md).
// =====================================================

// Prepara el sensor. Llamar una vez en setup().
void ecgIniciar();

// Devuelve el valor medido en milivolts.
float ecgLeerValor();

// La pregunta que le importa a la maquina de estados.
// true = hay senal cardiaca | false = no hay
bool ecgHaySenal();

#endif
