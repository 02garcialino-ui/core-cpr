#include "logica_fsm.h"
#include "sensor_ecg.h"
#include "sensor_ppg.h"
#include "config.h"
#include "logger.h"

static EstadoFsm estadoActual = FSM_MONITOREANDO;
static unsigned long tiempoInicioEstado = 0;

// T7.2 Parte B: una vez que el usuario confirma a mano "NO hay paro" con
// el PPG fallando, no se le vuelve a preguntar cada 500ms mientras la
// falla siga activa -- se confia en su palabra hasta que el mismo
// presione "Reintentar lectura" (T6.4, ver fsmReintentarConfirmacion()).
static bool confirmacionSilenciada = false;

void fsmIniciar() {
  estadoActual = FSM_MONITOREANDO;
  tiempoInicioEstado = millis();
  confirmacionSilenciada = false;
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
      if (confirmacionSilenciada) {
        // ya se confirmo a mano "NO hay paro" con esta misma falla activa
        // (T7.2 Parte B): no interrumpir de nuevo hasta "Reintentar lectura".
      } else if (ppgSensorFallo()) {
        logMsg(LOG_WARN, "FSM", "MONITOREANDO -> CONFIRMANDO_FALLO (el PPG fallo, no se asume el paro solo)");
        estadoActual = FSM_CONFIRMANDO_FALLO;
        tiempoInicioEstado = millis();
      } else {
        logMsg(LOG_WARN, "FSM", "MONITOREANDO -> COMPRIMIENDO (se perdio el pulso)");
        estadoActual = FSM_COMPRIMIENDO;
        tiempoInicioEstado = millis();
      }
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
      } else if (!confirmacionSilenciada && ppgSensorFallo()) {
        logMsg(LOG_WARN, "FSM", "REEVALUANDO -> CONFIRMANDO_FALLO (el PPG fallo, no se asume el paro solo)");
        estadoActual = FSM_CONFIRMANDO_FALLO;
      } else {
        logMsg(LOG_WARN, "FSM", "REEVALUANDO -> COMPRIMIENDO (sigue sin pulso, continua otro ciclo)");
        estadoActual = FSM_COMPRIMIENDO;
      }
      tiempoInicioEstado = millis();
    }

  } else if (estadoActual == FSM_CONFIRMANDO_FALLO) {
    // Fail-safe por tiempo (T7.2 Parte B): si nadie confirma a mano
    // (fsmConfirmarParo()/fsmDescartarParo(), llamados desde
    // firmware-esp32.ino cuando llega el toque de la pantalla), arranca
    // solo. Mismo criterio AHA que "ante la duda, comprimir".
    if (transcurrido >= DURACION_CONFIRMACION_MS) {
      logMsg(LOG_WARN, "FSM", "CONFIRMANDO_FALLO -> COMPRIMIENDO (nadie confirmo en " +
                              String(DURACION_CONFIRMACION_MS / 1000) + "s, fail-safe: se asume el paro)");
      estadoActual = FSM_COMPRIMIENDO;
      tiempoInicioEstado = millis();
    }
  }
}

EstadoFsm fsmEstadoActual() {
  return estadoActual;
}

void fsmConfirmarParo() {
  if (estadoActual != FSM_CONFIRMANDO_FALLO) return;
  logMsg(LOG_WARN, "FSM", "CONFIRMANDO_FALLO -> COMPRIMIENDO (confirmado a mano: SI hay paro)");
  estadoActual = FSM_COMPRIMIENDO;
  tiempoInicioEstado = millis();
}

void fsmDescartarParo() {
  if (estadoActual != FSM_CONFIRMANDO_FALLO) return;
  logMsg(LOG_INFO, "FSM", "CONFIRMANDO_FALLO -> MONITOREANDO (confirmado a mano: NO hay paro; se silencia hasta 'Reintentar lectura')");
  confirmacionSilenciada = true;
  estadoActual = FSM_MONITOREANDO;
  tiempoInicioEstado = millis();
}

void fsmReintentarConfirmacion() {
  if (!confirmacionSilenciada) return;
  confirmacionSilenciada = false;
  logMsg(LOG_INFO, "FSM", "Confirmacion de fallo del PPG reactivada (Reintentar lectura)");
}
