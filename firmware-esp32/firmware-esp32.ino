// =====================================================
//  Core CPR - Firmware principal (ESP32)
//  Estado: Fase 2 - T2.4 sensor de profundidad (simulado)
// =====================================================

#include "config.h"
#include "logger.h"
#include "sensor_ecg.h"
#include "sensor_ppg.h"
#include "sensor_fuerza.h"
#include "sensor_profundidad.h"

// Guardamos el estado anterior para avisar SOLO cuando cambia.
// Si no, la consola se llena de mensajes repetidos y no se ve nada.
bool habiaSenalEcg = false;
bool habiaPulso = false;
bool habiaExcesoFuerza = false;
bool habiaExcesoProfundidad = false;
bool primeraVuelta = true;

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

  ecgIniciar();
  ppgIniciar();
  fuerzaIniciar();
  profundidadIniciar();
  logMsg(LOG_INFO, "MAIN", "Gira los potenciometros o ajusta el HX711 para simular");
}

void loop() {
  bool haySenalEcg = ecgHaySenal();
  bool hayPulso = ppgHayPulso();
  bool excedeFuerza = fuerzaExcedeLimite();
  bool excedeProfundidad = profundidadExcedeLimite();

  // Cada LED es la senal visual de su propio sensor.
  digitalWrite(PIN_LED_ESTADO, haySenalEcg ? HIGH : LOW);
  digitalWrite(PIN_LED_PPG, hayPulso ? HIGH : LOW);
  digitalWrite(PIN_LED_FUERZA, excedeFuerza ? HIGH : LOW);
  digitalWrite(PIN_LED_PROFUNDIDAD, excedeProfundidad ? HIGH : LOW);

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
