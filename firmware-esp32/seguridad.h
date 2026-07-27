#ifndef SEGURIDAD_H
#define SEGURIDAD_H

#include <Arduino.h>

// =====================================================
//  Modulo de seguridad: paro de emergencia (T7.1)
//
//  El boton real es tipo "hongo NC" (normalmente cerrado, ver BOM): el
//  circuito esta CERRADO (pin en LOW) mientras no se presiona. Al
//  presionarlo, o si el cable se corta/desconecta, el circuito se ABRE
//  (pin en HIGH) -- misma reaccion en ambos casos, a proposito: una falla
//  de cableado se trata igual que una emergencia real, nunca pasa
//  desapercibida dejando el motor andando a ciegas.
// =====================================================

// Configura el pin. Llamar una vez en setup().
void seguridadIniciar();

// true = hay que detenerse (boton presionado O circuito abierto por falla).
// Es una lectura simple, sin logs: se llama en cada vuelta de loop(), asi
// que registrar cada llamada inundaria la consola.
bool paroEmergenciaActivo();

#endif
