#ifndef SELECTOR_MODO_H
#define SELECTOR_MODO_H

#include <Arduino.h>

// =====================================================
//  Selector de modo de paciente (switch fisico, T4.3)
//
//  Modo NINO: unico modo funcional. Arranca el diagnostico
//  y las lecturas de sensores.
//  Modo ADULTO: no funcional (el NEMA 23 no tiene fuerza
//  suficiente para comprimir a un adulto). El dispositivo
//  no hace ninguna lectura ni accion.
//
//  El modo paciente embarazada se descarto de esta version
//  por la misma limitacion del motor; queda como nota a
//  futuro en PLAN-PROYECTO.md, no como opcion del switch.
//
//  A diferencia de los sensores, un switch fisico se comporta
//  igual en Wokwi que en hardware real: no hace falta la
//  "doble capa" simulado/real.
// =====================================================

enum ModoPaciente {
  MODO_NINO,
  MODO_ADULTO
};

// Prepara el pin del switch. Llamar una vez en setup().
void selectorModoIniciar();

// Lee la posicion del switch.
// GND (LOW, con INPUT_PULLUP) = modo NINO.
ModoPaciente selectorModoLeer();

#endif
