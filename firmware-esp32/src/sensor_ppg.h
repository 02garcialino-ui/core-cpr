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

// Devuelve el valor medido. OJO: la unidad cambia segun el modo -- en
// MODO_SIMULACION son milivolts (0-3300, potenciometro); en real es la
// cuenta cruda de infrarrojo del MAX30102 (no son unidades comparables
// entre si, por eso hay un umbral distinto para cada caso en config.h).
float ppgLeerValor();

// La pregunta que le importa a la maquina de estados.
// true = hay pulso | false = no hay
bool ppgHayPulso();

// Distingue si "no hay pulso" es un paro real o el sensor que fallo
// (T7.2 Parte B). ppgHayPulso() trata ambos casos igual (false) porque
// a la mayoria de quien lo llama no le importa la diferencia; esta
// funcion es para el unico lugar que si le importa: decidir si hay que
// preguntar antes de comprimir en vez de asumir el paro solo.
bool ppgSensorFallo();

#endif
