// =====================================================
//  Core CPR - Firmware principal (ESP32)
//  Estado: Fase 7 - T7.1 paro de emergencia (adelantada; Fase 6 en pausa)
//  El motor corre un ciclo de compresion continuo en paralelo al resto
//  del programa; todavia no esta conectado a la FSM (eso viene en
//  fases posteriores de integracion, Fase 8).
// =====================================================

#include "config.h"
#include "logger.h"
#include "sensor_ecg.h"
#include "sensor_ppg.h"
#include "sensor_fuerza.h"
#include "sensor_profundidad.h"
#include "actuador_motor.h"
#include "logica_fsm.h"
#include "selector_modo.h"
#include "seguridad.h"

// Guardamos el estado anterior para avisar SOLO cuando cambia.
// Si no, la consola se llena de mensajes repetidos y no se ve nada.
bool habiaSenalEcg = false;
bool habiaPulso = false;
bool habiaExcesoFuerza = false;
bool habiaExcesoProfundidad = false;
bool habiaParoEmergencia = false;
bool primeraVuelta = true;
ModoPaciente modoAnterior = MODO_ADULTO;
bool primerModo = true;

void setup() {
  loggerIniciar();

  pinMode(PIN_LED_ESTADO, OUTPUT);
  pinMode(PIN_LED_PPG, OUTPUT);
  pinMode(PIN_LED_FUERZA, OUTPUT);
  pinMode(PIN_LED_PROFUNDIDAD, OUTPUT);
  pinMode(PIN_LED_COMPRESION, OUTPUT);
  logMsg(LOG_INFO, "MAIN", "LED de estado (ECG) listo en GPIO " + String(PIN_LED_ESTADO));
  logMsg(LOG_INFO, "MAIN", "LED de pulso (PPG) listo en GPIO " + String(PIN_LED_PPG));
  logMsg(LOG_INFO, "MAIN", "LED de alarma (fuerza) listo en GPIO " + String(PIN_LED_FUERZA));
  logMsg(LOG_INFO, "MAIN", "LED de alarma (profundidad) listo en GPIO " + String(PIN_LED_PROFUNDIDAD));
  logMsg(LOG_INFO, "MAIN", "LED de compresion (reemplaza al motor) listo en GPIO " + String(PIN_LED_COMPRESION));

  if (MODO_SIMULACION) {
    logMsg(LOG_WARN, "MAIN", "MODO_SIMULACION activo: los datos NO son de sensores reales");
  }

  selectorModoIniciar();
  ecgIniciar();
  ppgIniciar();
  fuerzaIniciar();
  profundidadIniciar();
  motorIniciar();
  seguridadIniciar();
  fsmIniciar();
  logMsg(LOG_INFO, "MAIN", "Gira los potenciometros o ajusta el HX711 para simular");

  // ---- Prueba T5.2: homing (busca el extremo retraido, define el cero) ----
  bool homingOk = motorHoming();
  if (homingOk) {
    logMsg(LOG_INFO, "MAIN", "Homing OK: motor en posicion cero (extremo retraido)");
  } else {
    logMsg(LOG_ERROR, "MAIN", "Homing fallo: revisar el switch de home antes de continuar");
  }

  // ---- Prueba T5.3: ciclo de compresion continuo (110 CPM, 5cm, suave) ----
  // Reemplaza la prueba puntual de T5.1 (ya validada). Esta corre sin
  // parar, en paralelo al resto del programa (ver motorPruebaCicloActualizar
  // en loop()). Independiente de la FSM (Fase 8 la conecta de verdad).
  logMsg(LOG_INFO, "MAIN", "Prueba T5.3: ciclo de compresion continuo a " +
                           String(RITMO_CPM_OBJETIVO) + " CPM");
  motorPruebaCicloIniciar();
}

void loop() {
  // El paro de emergencia se revisa PRIMERO y en cada vuelta (no espera al
  // bloque de 500ms): si esta activo, corta el motor de inmediato y no
  // deja que la prueba de ciclo avance ni un paso mas.
  bool paroActivo = paroEmergenciaActivo();
  if (paroActivo) {
    motorHabilitar(false);
  } else {
    // Avanza el motor si ya toca el proximo paso (no bloquea). Corre
    // siempre, sin importar el modo, para no cortar un movimiento a
    // mitad de camino.
    motorPruebaCicloActualizar();
  }

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
    return;   // modo no funcional: no se lee ningun sensor ni corre la FSM
  }

  // El resto (FSM, sensores, logs) se revisa cada ~500ms, no en cada vuelta
  // del loop(): ya no usamos delay() porque bloquearia el paso del motor,
  // que necesita revisarse muchas veces por segundo para moverse suave.
  static unsigned long ultimoChequeoMs = 0;
  if (millis() - ultimoChequeoMs < 500) {
    return;
  }
  ultimoChequeoMs = millis();

  if (primeraVuelta || paroActivo != habiaParoEmergencia) {
    if (paroActivo) {
      logMsg(LOG_ERROR, "SEGURIDAD", ">>> PARO DE EMERGENCIA ACTIVO: motor detenido");
    } else {
      logMsg(LOG_INFO, "SEGURIDAD", ">>> Paro de emergencia liberado, motor puede operar");
    }
    habiaParoEmergencia = paroActivo;
  }

  fsmActualizar();

  // Mientras esta COMPRIMIENDO no se lee ni se muestra ECG/PPG: el
  // movimiento ensucia esas lecturas (ver docs/diagramas/T4.1-fsm.md).
  // Fuerza y profundidad son sensores de control, no de diagnostico:
  // esos si se leen siempre, incluso durante la compresion.
  bool comprimiendo = (fsmEstadoActual() == FSM_COMPRIMIENDO);
  digitalWrite(PIN_LED_COMPRESION, comprimiendo ? HIGH : LOW);

  bool excedeFuerza = fuerzaExcedeLimite();
  bool excedeProfundidad = profundidadExcedeLimite();
  digitalWrite(PIN_LED_FUERZA, excedeFuerza ? HIGH : LOW);
  digitalWrite(PIN_LED_PROFUNDIDAD, excedeProfundidad ? HIGH : LOW);

  // ---- Cruce de posicion (T5.4 / Punto E) ----
  // Compara lo que el motor "cree" (conteo de pasos desde el home) contra
  // lo que mide el sensor de verdad (VL53L0X). En esta prueba aislada el
  // home se toma como si fuera el punto de contacto con el pecho (todavia
  // no definimos POSICION_LISTO_CM, la distancia real de aproximacion).
  float posicionMotor = motorPosicionCm();
  float posicionSensor = profundidadLeerCm();
  logMsg(LOG_DEBUG, "MOTOR", "posicion calculada=" + String(posicionMotor, 1) +
                             "cm  sensor VL53L0X=" + String(posicionSensor, 1) + "cm");
  if (fabs(posicionMotor - posicionSensor) > TOLERANCIA_POSICION_CM) {
    logMsg(LOG_WARN, "MOTOR", "Diferencia entre posicion calculada y el sensor "
                              "de profundidad (posible perdida de pasos, o en "
                              "Wokwi falta girar el potenciometro a mano)");
  }

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
}
