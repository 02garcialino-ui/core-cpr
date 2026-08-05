#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

// =====================================================
//  Modulo de alarmas acusticas (T7.3)
//
//  Buzzer simple on/off (sin PWM): los tres patrones se logran
//  prendiendo y apagando el pin a distinto ritmo, no con distintos
//  tonos. Se maneja igual que el motor (T5.3): sin delay(), se
//  actualiza en cada vuelta de loop() para no bloquear el resto del
//  programa.
// =====================================================

enum PatronBuzzer {
  BUZZER_NINGUNO,   // apagado
  BUZZER_VIVO,      // pitido corto, una sola vez (confirmacion)
  BUZZER_FALLO,     // pitidos cortos y rapidos, repetidos
  BUZZER_LIMITE     // tono fijo continuo
};

// Configura el pin. Llamar una vez en setup().
void buzzerIniciar();

// Le dice al buzzer que alarma tiene que sonar. Si el patron no cambio
// respecto del anterior, no hace nada (para que BUZZER_VIVO no repita
// el pitido en cada llamada mientras se mantiene el mismo estado).
void buzzerFijarPatron(PatronBuzzer patron);

// Avanza el patron actual segun el tiempo transcurrido. Llamar en CADA
// vuelta de loop() (no solo cada 500ms), igual que el motor, para que
// los pitidos rapidos de BUZZER_FALLO se escuchen bien.
void buzzerActualizar();

#endif
