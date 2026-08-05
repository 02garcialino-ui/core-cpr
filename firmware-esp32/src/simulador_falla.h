#ifndef SIMULADOR_FALLA_H
#define SIMULADOR_FALLA_H

#include <Arduino.h>

// =====================================================
//  SOLO PARA PRUEBAS (T7.2): simula que un sensor fallo (NAN).
//
//  Los sensores simulados (potenciometros/HX711 de Wokwi) nunca fallan
//  solos, asi que este switch de prueba fuerza la falla a mano para
//  poder probar el aborto del motor y (mas adelante) la confirmacion
//  de PPG. Un solo switch para los 3 sensores (PPG, fuerza, profundidad)
//  por falta de pines libres -- no busca ser realista, solo probar la
//  reaccion del sistema ante una falla.
//
//  No existe en hardware real: solo tiene efecto dentro de
//  MODO_SIMULACION (ver config.h).
// =====================================================

// Prepara el pin. Llamar una vez en setup().
void simuladorFallaIniciar();

// true = el switch de prueba esta activo (forzar falla en los 3 sensores).
bool simuladorFallaActiva();

#endif
