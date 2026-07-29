#ifndef HMI_PANTALLA_H
#define HMI_PANTALLA_H

#include <Arduino.h>
#include "selector_modo.h"
#include "logica_fsm.h"

// =====================================================
//  Modulo de la pantalla Nextion (T6.1 / T6.3)
//
//  La pantalla NO elige el modo (eso lo hace el switch fisico, ver
//  selector_modo.h / T4.3): solo confirma cual modo quedo activo.
//  El modo ADULTO no es funcional, asi que la pantalla lo avisa en
//  vez de operar.
//
//  T6.3 agrega el estado clinico simple (VIVO / PARO - COMPRIMIENDO /
//  REEVALUANDO) y la grafica tipo osciloscopio del ECG, en el
//  componente Waveform "s0" de la pagina "inicio".
//
//  Protocolo Nextion: se le manda texto plano por un puerto serial
//  aparte (Serial2), terminado siempre en 3 bytes 0xFF. Wokwi no
//  simula esta pantalla, asi que cada comando tambien se imprime
//  con logMsg para poder verificarlo en la consola durante Fase 6.
// =====================================================

// Prepara el puerto serial hacia la pantalla y muestra la pagina inicial.
// Llamar una vez en setup().
void hmiIniciar();

// Actualiza el texto de la pantalla con el modo de paciente activo.
void hmiMostrarModo(ModoPaciente modo);

// Actualiza el texto de estado clinico segun el estado de la FSM.
void hmiMostrarEstado(EstadoFsm estado);

// Agrega un punto a la grafica del ECG (componente Waveform "s0").
// Llamar solo cuando NO se esta comprimiendo (el movimiento ensucia
// la lectura del ECG, ver firmware-esp32.ino).
void hmiGraficarEcg(float valorMv);

#endif
