#ifndef SEGURIDAD_H
#define SEGURIDAD_H

#include <Arduino.h>

// =====================================================
//  Modulo de seguridad: paro de emergencia (T7.1) +
//  boton "Reintentar lectura" (T6.4)
//
//  El boton de paro real es tipo "hongo NC" (normalmente cerrado, ver
//  BOM): el circuito esta CERRADO (pin en LOW) mientras no se presiona.
//  Al presionarlo, o si el cable se corta/desconecta, el circuito se
//  ABRE (pin en HIGH) -- misma reaccion en ambos casos, a proposito: una
//  falla de cableado se trata igual que una emergencia real, nunca pasa
//  desapercibida dejando el motor andando a ciegas.
//
//  El boton de reintentar es un pulsador normal (no NC). Por ahora solo
//  detecta el click (con antirebote); la accion real de reintentar el
//  sensor que fallo se conecta en T7.2, cuando exista la logica de
//  fallo de sensores.
// =====================================================

// Configura los pines. Llamar una vez en setup().
void seguridadIniciar();

// true = hay que detenerse (boton presionado O circuito abierto por falla).
// Es una lectura simple, sin logs: se llama en cada vuelta de loop(), asi
// que registrar cada llamada inundaria la consola.
bool paroEmergenciaActivo();

// true UNA SOLA VEZ por click (flanco de bajada, con antirebote). Llamar
// en cada vuelta de loop(), igual que paroEmergenciaActivo().
bool botonReintentarPresionado();

#endif
