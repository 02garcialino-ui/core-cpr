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
//  FSM_CONFIRMANDO_FALLO (T7.2 Parte B): si el PPG FALLA (NAN) antes
//  de comprimir, no se trata igual que "no hay pulso" real -- no se
//  asume el paro solo. Se pide confirmar a mano (pantalla, T7.2 Parte
//  B) y, si nadie confirma en DURACION_CONFIRMACION_MS, arranca solo
//  (fail-safe). fsmConfirmarParo()/fsmDescartarParo() los llama
//  firmware-esp32.ino cuando llega el toque de la pantalla;
//  fsmReintentarConfirmacion() se llama desde el boton "Reintentar
//  lectura" (T6.4).
//
//  DETENIDO_EMERGENCIA (Fase 7) se agrega mas adelante.
// =====================================================

enum EstadoFsm {
  FSM_MONITOREANDO,
  FSM_COMPRIMIENDO,
  FSM_REEVALUANDO,
  FSM_CONFIRMANDO_FALLO
};

// Prepara la FSM. Llamar una vez en setup().
void fsmIniciar();

// Lee los sensores, decide y actualiza el estado. Llamar en loop().
void fsmActualizar();

// El estado actual, para que otros modulos (ej. el LED de prueba de T4.5)
// sepan que hacer.
EstadoFsm fsmEstadoActual();

// Boton tactil "Si hay paro" (T7.2 Parte B). Solo tiene efecto en
// FSM_CONFIRMANDO_FALLO: pasa a COMPRIMIENDO de inmediato.
void fsmConfirmarParo();

// Boton tactil "No hay paro" (T7.2 Parte B). Solo tiene efecto en
// FSM_CONFIRMANDO_FALLO: vuelve a MONITOREANDO y silencia la
// confirmacion (no se vuelve a preguntar aunque el PPG siga fallando)
// hasta que se llame a fsmReintentarConfirmacion().
void fsmDescartarParo();

// Limpia el silencio de confirmacion. Llamar cuando se presiona
// "Reintentar lectura" (T6.4): el proximo chequeo vuelve a confiar en
// el PPG y, si sigue fallando, pregunta de nuevo.
void fsmReintentarConfirmacion();

#endif
