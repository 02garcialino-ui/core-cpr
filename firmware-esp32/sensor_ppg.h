#ifndef SENSOR_PPG_H
#define SENSOR_PPG_H

#include <Arduino.h>

// =====================================================
//  Modulo del PPG (fotopletismografia)
//
//  Responde una sola pregunta al resto del sistema:
//  "hay pulso, si o no?"
//
//  Quien lo llama NO sabe ni le importa si el dato viene de un
//  potenciometro simulado o del MAX30102 real. Eso lo decide
//  MODO_SIMULACION dentro de este archivo (ver CONVENCIONES-CODIGO.md).
// =====================================================

// Prepara el sensor. Llamar una vez en setup().
void ppgIniciar();

// Devuelve el valor medido en milivolts.
float ppgLeerValor();

// La pregunta que le importa a la maquina de estados.
// true = hay pulso | false = no hay
bool ppgHayPulso();

#endif
