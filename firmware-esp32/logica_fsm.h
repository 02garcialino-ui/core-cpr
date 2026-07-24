#ifndef LOGICA_FSM_H
#define LOGICA_FSM_H

#include <Arduino.h>

// =====================================================
//  Modulo de logica de decision (maquina de estados)
//
//  Decide si el paciente esta "vivo" o en "paro", cruzando
//  ECG y PPG. El PPG tiene la ultima palabra (ver T4.1 y
//  docs/diagramas/T4.1-fsm.md): el ECG solo queda registrado
//  en los logs, por si aparece un caso de AESP (actividad
//  electrica sin pulso).
//
//  DETENIDO_EMERGENCIA (Fase 7) se agrega mas adelante.
// =====================================================

enum EstadoFsm {
  FSM_MONITOREANDO,
  FSM_COMPRIMIENDO,
  FSM_REEVALUANDO
};

// Prepara la FSM. Llamar una vez en setup().
void fsmIniciar();

// Lee los sensores, decide y actualiza el estado. Llamar en loop().
void fsmActualizar();

// El estado actual, para que otros modulos (ej. el LED de prueba de T4.5)
// sepan que hacer.
EstadoFsm fsmEstadoActual();

#endif
