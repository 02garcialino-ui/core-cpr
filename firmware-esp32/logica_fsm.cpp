#include "logica_fsm.h"
#include "sensor_ecg.h"
#include "sensor_ppg.h"
#include "config.h"
#include "logger.h"

static EstadoFsm estadoActual = FSM_MONITOREANDO;
static unsigned long tiempoInicioEstado = 0;

void fsmIniciar() {
  estadoActual = FSM_MONITOREANDO;
  tiempoInicioEstado = millis();
  logMsg(LOG_INFO, "FSM", "Estado inicial: MONITOREANDO");
}

// El PPG tiene la ultima palabra (ver T4.1): el corazon puede tener
// actividad electrica sin bombear sangre (AESP), asi que el ECG solo
// se registra en su propio log, nunca decide.
void fsmActualizar() {
  unsigned long transcurrido = millis() - tiempoInicioEstado;

  if (estadoActual == FSM_MONITOREANDO) {
    bool hayPulso = ppgHayPulso();
    ecgHaySenal();  // solo se registra en su propio log, no decide

    if (!hayPulso) {
      logMsg(LOG_WARN, "FSM", "MONITOREANDO -> COMPRIMIENDO (se perdio el pulso)");
      estadoActual = FSM_COMPRIMIENDO;
      tiempoInicioEstado = millis();
    }

  } else if (estadoActual == FSM_COMPRIMIENDO) {
    // No se lee ECG ni PPG aqui: el movimiento de las compresiones
    // ensucia ambas lecturas (ver docs/diagramas/T4.1-fsm.md).
    if (transcurrido >= DURACION_COMPRESION_MS) {
      logMsg(LOG_INFO, "FSM", "COMPRIMIENDO -> REEVALUANDO (se cumplio el ciclo, pausa para revisar)");
      estadoActual = FSM_REEVALUANDO;
      tiempoInicioEstado = millis();
    }

  } else if (estadoActual == FSM_REEVALUANDO) {
    if (transcurrido >= DURACION_REEVALUACION_MS) {
      bool hayPulso = ppgHayPulso();
      ecgHaySenal();  // solo se registra en su propio log, no decide

      if (hayPulso) {
        logMsg(LOG_INFO, "FSM", "REEVALUANDO -> MONITOREANDO (recupero el pulso)");
        estadoActual = FSM_MONITOREANDO;
      } else {
        logMsg(LOG_WARN, "FSM", "REEVALUANDO -> COMPRIMIENDO (sigue sin pulso, continua otro ciclo)");
        estadoActual = FSM_COMPRIMIENDO;
      }
      tiempoInicioEstado = millis();
    }
  }
}

EstadoFsm fsmEstadoActual() {
  return estadoActual;
}
