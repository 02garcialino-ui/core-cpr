// =====================================================
//  Core CPR - Firmware principal (ESP32)
//  Estado: Fase 4 - T4.4 FSM con reevaluacion (COMPRIMIENDO/REEVALUANDO)
//  (todavia sin salida de motor ni LED de compresion, ver T4.5)
// =====================================================

#include "config.h"
#include "logger.h"
#include "sensor_ecg.h"
#include "sensor_ppg.h"
#include "sensor_fuerza.h"
#include "sensor_profundidad.h"
#include "logica_fsm.h"
#include "selector_modo.h"

// Guardamos el estado anterior para avisar SOLO cuando cambia.
// Si no, la consola se llena de mensajes repetidos y no se ve nada.
bool habiaSenalEcg = false;
bool habiaPulso = false;
bool habiaExcesoFuerza = false;
bool habiaExcesoProfundidad = false;
bool primeraVuelta = true;
ModoPaciente modoAnterior = MODO_ADULTO;
bool primerModo = true;

void setup() {
  loggerIniciar();

  pinMode(PIN_LED_ESTADO, OUTPUT);
  pinMode(PIN_LED_PPG, OUTPUT);
  pinMode(PIN_LED_FUERZA, OUTPUT);
  pinMode(PIN_LED_PROFUNDIDAD, OUTPUT);
  logMsg(LOG_INFO, "MAIN", "LED de estado (ECG) listo en GPIO " + String(PIN_LED_ESTADO));
  logMsg(LOG_INFO, "MAIN", "LED de pulso (PPG) listo en GPIO " + String(PIN_LED_PPG));
  logMsg(LOG_INFO, "MAIN", "LED de alarma (fuerza) listo en GPIO " + String(PIN_LED_FUERZA));
  logMsg(LOG_INFO, "MAIN", "LED de alarma (profundidad) listo en GPIO " + String(PIN_LED_PROFUNDIDAD));

  if (MODO_SIMULACION) {
    logMsg(LOG_WARN, "MAIN", "MODO_SIMULACION activo: los datos NO son de sensores reales");
  }

  selectorModoIniciar();
  ecgIniciar();
  ppgIniciar();
  fuerzaIniciar();
  profundidadIniciar();
  fsmIniciar();
  logMsg(LOG_INFO, "MAIN", "Gira los potenciometros o ajusta el HX711 para simular");
}

void loop() {
  ModoPaciente modoActual = selectorModoLeer();

  if (primerModo || modoActual != modoAnterior) {
    if (modoActual == MODO_NINO) {
      logMsg(LOG_INFO, "MAIN", ">>> MODO NINO seleccionado: iniciando diagnostico");
    } else {
      logMsg(LOG_WARN, "MAIN", ">>> MODO ADULTO seleccionado: Modo no disponible (mensaje de pantalla, Fase 6)");
    }
    modoAnterior = modoActual;
    primerModo = false;
  }

  if (modoActual == MODO_ADULTO) {
    delay(500);
    return;   // modo no funcional: no se lee ningun sensor ni corre la FSM
  }

  fsmActualizar();

  // Mientras esta COMPRIMIENDO no se lee ni se muestra ECG/PPG: el
  // movimiento ensucia esas lecturas (ver docs/diagramas/T4.1-fsm.md).
  // Fuerza y profundidad son sensores de control, no de diagnostico:
  // esos si se leen siempre, incluso durante la compresion.
  bool comprimiendo = (fsmEstadoActual() == FSM_COMPRIMIENDO);

  bool excedeFuerza = fuerzaExcedeLimite();
  bool excedeProfundidad = profundidadExcedeLimite();
  digitalWrite(PIN_LED_FUERZA, excedeFuerza ? HIGH : LOW);
  digitalWrite(PIN_LED_PROFUNDIDAD, excedeProfundidad ? HIGH : LOW);

  if (!comprimiendo) {
    bool haySenalEcg = ecgHaySenal();
    bool hayPulso = ppgHayPulso();

    digitalWrite(PIN_LED_ESTADO, haySenalEcg ? HIGH : LOW);
    digitalWrite(PIN_LED_PPG, hayPulso ? HIGH : LOW);

    if (primeraVuelta || haySenalEcg != habiaSenalEcg) {
      if (haySenalEcg) {
        logMsg(LOG_INFO, "MAIN", ">>> HAY SENAL CARDIACA (paciente con actividad)");
      } else {
        logMsg(LOG_WARN, "MAIN", ">>> SIN SENAL CARDIACA (posible paro)");
      }
      habiaSenalEcg = haySenalEcg;
    }

    if (primeraVuelta || hayPulso != habiaPulso) {
      if (hayPulso) {
        logMsg(LOG_INFO, "MAIN", ">>> HAY PULSO (paciente con actividad)");
      } else {
        logMsg(LOG_WARN, "MAIN", ">>> SIN PULSO (posible paro)");
      }
      habiaPulso = hayPulso;
    }
  }

  if (primeraVuelta || excedeFuerza != habiaExcesoFuerza) {
    if (excedeFuerza) {
      logMsg(LOG_WARN, "MAIN", ">>> FUERZA EXCEDIDA (riesgo de lesion)");
    } else {
      logMsg(LOG_INFO, "MAIN", ">>> FUERZA DENTRO DEL LIMITE");
    }
    habiaExcesoFuerza = excedeFuerza;
  }

  if (primeraVuelta || excedeProfundidad != habiaExcesoProfundidad) {
    if (excedeProfundidad) {
      logMsg(LOG_WARN, "MAIN", ">>> PROFUNDIDAD EXCEDIDA (riesgo de lesion)");
    } else {
      logMsg(LOG_INFO, "MAIN", ">>> PROFUNDIDAD DENTRO DEL LIMITE");
    }
    habiaExcesoProfundidad = excedeProfundidad;
  }

  primeraVuelta = false;

  delay(500);   // media lectura por segundo alcanza para ver que pasa
}
