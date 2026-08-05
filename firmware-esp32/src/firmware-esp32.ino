// =====================================================
//  Core CPR - Firmware principal (ESP32)
//  Estado: Fase 8 - T8.1 (el motor ya esta conectado a la FSM: arranca
//  al entrar a FSM_COMPRIMIENDO y, al salir, retrae el piston desde la
//  posicion actual en vez de terminar la bajada completa. Ver
//  motorCicloIniciar/Actualizar/Detener en actuador_motor.h)
//  T7.3 (buzzer: alarmas acusticas segun prioridad -- limite excedido >
//  fallo de sensor > paciente vivo. Ver buzzer.h)
//  T7.2 Parte B: confirmacion de fallo del PPG antes de comprimir
//  (pantalla de confirmacion + fail-safe de 10s). El toque real de los
//  botones queda sin probar en Wokwi, ver hmi_pantalla.h
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
#include "hmi_pantalla.h"
#include "simulador_falla.h"
#include "buzzer.h"

// Guardamos el estado anterior para avisar SOLO cuando cambia.
// Si no, la consola se llena de mensajes repetidos y no se ve nada.
bool habiaSenalEcg = false;
bool habiaPulso = false;
bool habiaExcesoFuerza = false;
bool habiaExcesoProfundidad = false;
bool habiaParoEmergencia = false;
bool habiaAbortoPorLimite = false;
bool abortoPorLimite = false;   // se actualiza cada 500ms, se usa en cada vuelta
bool primeraVuelta = true;
ModoPaciente modoAnterior = MODO_ADULTO;
bool primerModo = true;
EstadoFsm estadoAnterior = FSM_MONITOREANDO;
bool primerEstado = true;

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
  simuladorFallaIniciar();
  hmiIniciar();
  ecgIniciar();
  ppgIniciar();
  fuerzaIniciar();
  profundidadIniciar();
  motorIniciar();
  seguridadIniciar();
  buzzerIniciar();
  fsmIniciar();
  logMsg(LOG_INFO, "MAIN", "Gira los potenciometros o ajusta el HX711 para simular");

  // ---- Prueba T5.2: homing (busca el extremo retraido, define el cero) ----
  bool homingOk = motorHoming();
  if (homingOk) {
    logMsg(LOG_INFO, "MAIN", "Homing OK: motor en posicion cero (extremo retraido)");
  } else {
    logMsg(LOG_ERROR, "MAIN", "Homing fallo: revisar el switch de home antes de continuar");
  }

  logMsg(LOG_INFO, "MAIN", "Ciclo de compresion configurado a " +
                           String(RITMO_CPM_OBJETIVO) +
                           " CPM (arranca cuando la FSM entra a COMPRIMIENDO)");
}

void loop() {
  // El paro de emergencia se revisa PRIMERO y en cada vuelta (no espera al
  // bloque de 500ms): si esta activo, corta el motor de inmediato y no
  // deja que el ciclo avance ni un paso mas. Igual con el aborto por
  // limite (T7.2 Parte A). Estos dos casos cortan siempre en el
  // instante, a proposito -- no usan la retraccion suave de
  // motorCicloDetener() (ver T8.1).
  bool paroActivo = paroEmergenciaActivo();
  if (paroActivo || abortoPorLimite) {
    motorHabilitar(false);
  } else if (motorCicloEnMovimiento()) {
    // Avanza el motor si ya toca el proximo paso (no bloquea). Corre
    // siempre, sin importar el modo, para no cortar un movimiento a
    // mitad de camino. Sigue llamandose aunque la FSM ya haya salido de
    // COMPRIMIENDO, hasta que termine de retraerse.
    motorCicloActualizar();
  }

  // Igual que el motor: se actualiza en CADA vuelta de loop() (no cada
  // 500ms), para que los pitidos rapidos de BUZZER_FALLO se escuchen
  // bien y no queden "aplastados" por el chequeo lento de sensores.
  buzzerActualizar();

  // Se revisa en cada vuelta de loop() (no cada 500ms) para que el
  // antirebote de botonReintentarPresionado() tenga lecturas frecuentes.
  if (botonReintentarPresionado()) {
    logMsg(LOG_INFO, "SEGURIDAD", ">>> Reintentar lectura solicitado (boton presionado)");
    // T7.2 Parte B: si estaba silenciada la confirmacion de fallo del
    // PPG (el usuario ya habia dicho "NO hay paro" con el sensor roto),
    // este boton reactiva el chequeo normal.
    fsmReintentarConfirmacion();
  }

  ModoPaciente modoActual = selectorModoLeer();

  if (primerModo || modoActual != modoAnterior) {
    if (modoActual == MODO_NINO) {
      logMsg(LOG_INFO, "MAIN", ">>> MODO NINO seleccionado: iniciando diagnostico");
    } else {
      logMsg(LOG_WARN, "MAIN", ">>> MODO ADULTO seleccionado: Modo no disponible");
    }
    hmiMostrarModo(modoActual);
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
  EstadoFsm estadoActual = fsmEstadoActual();
  bool comprimiendo = (estadoActual == FSM_COMPRIMIENDO);
  digitalWrite(PIN_LED_COMPRESION, comprimiendo ? HIGH : LOW);

  if (primerEstado || estadoActual != estadoAnterior) {
    hmiMostrarEstado(estadoActual);

    // ---- Motor conectado a la FSM (T8.1) ----
    if (estadoActual == FSM_COMPRIMIENDO) {
      motorCicloIniciar();
    } else if (estadoAnterior == FSM_COMPRIMIENDO) {
      motorCicloDetener();
    }

    estadoAnterior = estadoActual;
    primerEstado = false;
  }

  // ---- Confirmacion de fallo del PPG (T7.2 Parte B) ----
  // Mientras se espera confirmacion, se revisa si llego un toque desde
  // la pantalla. Sin probar en Wokwi (no simula la Nextion real, ver
  // hmi_pantalla.h); se valida con hardware real en T9.4.
  if (estadoActual == FSM_CONFIRMANDO_FALLO) {
    ToqueNextion toque = hmiLeerToque();
    if (toque == TOQUE_SI_HAY_PARO) {
      fsmConfirmarParo();
    } else if (toque == TOQUE_NO_HAY_PARO) {
      fsmDescartarParo();
    }
  }

  bool excedeFuerza = fuerzaExcedeLimite();
  bool excedeProfundidad = profundidadExcedeLimite();
  digitalWrite(PIN_LED_FUERZA, excedeFuerza ? HIGH : LOW);
  digitalWrite(PIN_LED_PROFUNDIDAD, excedeProfundidad ? HIGH : LOW);

  // ---- Aborto por limite o falla de sensor (T7.2 Parte A) ----
  // Se actualiza aca (cada 500ms, junto con la lectura de los sensores)
  // pero se aplica en CADA vuelta de loop() (ver el bloque del paro de
  // emergencia, arriba), igual que T7.1: una vez detectado, el motor
  // queda deshabilitado hasta la proxima revision que lo confirme OK.
  abortoPorLimite = excedeFuerza || excedeProfundidad;
  if (primeraVuelta || abortoPorLimite != habiaAbortoPorLimite) {
    if (abortoPorLimite) {
      logMsg(LOG_ERROR, "SEGURIDAD", ">>> LIMITE O FALLA DE FUERZA/PROFUNDIDAD: motor abortado");
    } else {
      logMsg(LOG_INFO, "SEGURIDAD", ">>> Fuerza y profundidad OK, motor puede operar");
    }
    habiaAbortoPorLimite = abortoPorLimite;
  }

  // ---- Alarmas acusticas (T7.3) ----
  // Prioridad: limite excedido > fallo de sensor (esperando confirmacion)
  // > paciente vivo (solo confirma, no es una alarma real). Si ninguna
  // aplica, el buzzer queda en silencio.
  PatronBuzzer patronBuzzer = BUZZER_NINGUNO;
  if (abortoPorLimite) {
    patronBuzzer = BUZZER_LIMITE;
  } else if (estadoActual == FSM_CONFIRMANDO_FALLO) {
    patronBuzzer = BUZZER_FALLO;
  } else if (estadoActual == FSM_MONITOREANDO) {
    patronBuzzer = BUZZER_VIVO;
  }
  buzzerFijarPatron(patronBuzzer);

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
    hmiGraficarEcg(ecgLeerValor());

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
