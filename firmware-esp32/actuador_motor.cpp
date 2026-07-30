#include "actuador_motor.h"
#include "config.h"
#include "logger.h"

// ---- Estado del movimiento en curso (motorIniciarTramo/ActualizarTramo) ----
static long g_pasosTotalTramo = 0;
static long g_pasosHechosTramo = 0;
static long g_pasosRampa = 0;
static unsigned long g_pasoMinUs = 0;    // delay entre pasos en velocidad de crucero
static unsigned long g_pasoMaxUs = 0;    // delay al iniciar/terminar (mas lento)
static unsigned long g_proximoPasoUs = 0;
static bool g_sentidoAdelanteTramo = true;

// ---- Posicion contada desde el home (T5.4) ----
static long g_posicionPasos = 0;

// ---- Estado de la prueba de ciclo continuo (T5.3) ----
enum EstadoCicloPrueba { CICLO_BAJANDO, CICLO_SUBIENDO };
static EstadoCicloPrueba g_estadoCiclo = CICLO_BAJANDO;

// ---- Log de motorHabilitar() (solo cuando cambia, ver mas abajo) ----
static bool g_habilitadoAnterior = false;
static bool g_primeraLlamadaHabilitar = true;

void motorIniciar() {
  pinMode(PIN_MOTOR_STEP, OUTPUT);
  pinMode(PIN_MOTOR_DIR, OUTPUT);
  pinMode(PIN_MOTOR_ENABLE, OUTPUT);
  pinMode(PIN_FINAL_CARRERA_HOME, INPUT_PULLUP);

  digitalWrite(PIN_MOTOR_STEP, LOW);
  motorHabilitar(false);   // arranca deshabilitado, por seguridad

  logMsg(LOG_INFO, "MOTOR", "Driver listo (STEP=" + String(PIN_MOTOR_STEP) +
                            " DIR=" + String(PIN_MOTOR_DIR) +
                            " ENABLE=" + String(PIN_MOTOR_ENABLE) + ")");
  logMsg(LOG_INFO, "MOTOR", "Switch de home listo en GPIO " + String(PIN_FINAL_CARRERA_HOME));
}

void motorHabilitar(bool activar) {
  int nivelActivo = MOTOR_ENABLE_ACTIVO_BAJO ? LOW : HIGH;
  int nivelInactivo = MOTOR_ENABLE_ACTIVO_BAJO ? HIGH : LOW;
  digitalWrite(PIN_MOTOR_ENABLE, activar ? nivelActivo : nivelInactivo);

  // Se llama en cada vuelta de loop() mientras el paro de emergencia o el
  // aborto por limite (T7.1/T7.2 Parte A) estan activos: sin este filtro,
  // el log se repite miles de veces por segundo. Mismo patron "solo
  // cuando cambia" que ya se usa en firmware-esp32.ino.
  if (MODO_DEBUG && (g_primeraLlamadaHabilitar || activar != g_habilitadoAnterior)) {
    logMsg(LOG_DEBUG, "MOTOR", activar ? "habilitado" : "deshabilitado");
    g_habilitadoAnterior = activar;
    g_primeraLlamadaHabilitar = false;
  }
}

bool motorHoming() {
  logMsg(LOG_INFO, "MOTOR", "Iniciando homing (buscando el extremo retraido)");

  motorHabilitar(true);
  digitalWrite(PIN_MOTOR_DIR, LOW);   // LOW = sentido retraido (hacia el switch de home)

  long pasosRecorridos = 0;

  // Con INPUT_PULLUP, el switch da LOW cuando se activa (toca el tope).
  while (digitalRead(PIN_FINAL_CARRERA_HOME) == HIGH) {
    digitalWrite(PIN_MOTOR_STEP, HIGH);
    delayMicroseconds(MOTOR_PULSO_US);
    digitalWrite(PIN_MOTOR_STEP, LOW);
    delayMicroseconds(MOTOR_HOMING_DELAY_US);
    pasosRecorridos++;

    if (pasosRecorridos > HOMING_MAX_PASOS) {
      motorHabilitar(false);
      logMsg(LOG_ERROR, "MOTOR", "Homing fallo: no se encontro el switch tras " +
                                 String(pasosRecorridos) +
                                 " pasos. Revisar cableado del switch de home.");
      return false;
    }
  }

  motorHabilitar(false);
  g_posicionPasos = 0;   // aqui es donde el conteo empieza a valer: el home es el cero
  logMsg(LOG_INFO, "MOTOR", "Homing completo: extremo retraido encontrado en " +
                            String(pasosRecorridos) + " pasos");
  return true;
}

void motorIniciarTramo(long pasos, unsigned long duracionMs) {
  bool sentidoAdelante = (pasos >= 0);
  g_sentidoAdelanteTramo = sentidoAdelante;
  digitalWrite(PIN_MOTOR_DIR, sentidoAdelante ? HIGH : LOW);
  motorHabilitar(true);

  g_pasosTotalTramo = abs(pasos);
  g_pasosHechosTramo = 0;

  g_pasosRampa = (long)(g_pasosTotalTramo * FRACCION_RAMPA);
  if (g_pasosRampa < 1) g_pasosRampa = 1;

  unsigned long duracionUs = duracionMs * 1000UL;
  g_pasoMinUs = duracionUs / g_pasosTotalTramo;   // velocidad de crucero
  g_pasoMaxUs = g_pasoMinUs * 3;                  // al iniciar/terminar, 3x mas lento

  g_proximoPasoUs = micros();

  if (MODO_DEBUG) {
    logMsg(LOG_DEBUG, "MOTOR", "tramo iniciado: " + String(g_pasosTotalTramo) +
                               " pasos en " + String(duracionMs) + "ms, sentido=" +
                               (sentidoAdelante ? "adelante" : "atras"));
  }
}

bool motorActualizarTramo() {
  if (g_pasosHechosTramo >= g_pasosTotalTramo) {
    motorHabilitar(false);
    return false;   // tramo ya termino
  }

  // Resta "overflow-safe": aunque micros() de la vuelta (pasa cada ~70 min),
  // restar y convertir a long da el resultado correcto igual.
  if ((long)(micros() - g_proximoPasoUs) < 0) {
    return true;   // todavia no toca el siguiente paso
  }

  digitalWrite(PIN_MOTOR_STEP, HIGH);
  delayMicroseconds(MOTOR_PULSO_US);
  digitalWrite(PIN_MOTOR_STEP, LOW);

  g_pasosHechosTramo++;
  g_posicionPasos += g_sentidoAdelanteTramo ? 1 : -1;

  // Perfil trapezoidal: arranca lento, acelera a crucero, frena al final.
  long pasosRestantes = g_pasosTotalTramo - g_pasosHechosTramo;
  unsigned long delayActual;
  if (g_pasosHechosTramo < g_pasosRampa) {
    delayActual = map(g_pasosHechosTramo, 0, g_pasosRampa, g_pasoMaxUs, g_pasoMinUs);
  } else if (pasosRestantes < g_pasosRampa) {
    delayActual = map(pasosRestantes, 0, g_pasosRampa, g_pasoMaxUs, g_pasoMinUs);
  } else {
    delayActual = g_pasoMinUs;
  }

  g_proximoPasoUs = micros() + delayActual;
  return true;
}

void motorPruebaCicloIniciar() {
  g_estadoCiclo = CICLO_BAJANDO;
  motorIniciarTramo(PASOS_COMPRESION, DURACION_BAJADA_MS);
  logMsg(LOG_INFO, "MOTOR", "Prueba T5.3: ciclo de compresion iniciado (bajando)");
}

void motorPruebaCicloActualizar() {
  bool enMovimiento = motorActualizarTramo();
  if (enMovimiento) return;

  if (g_estadoCiclo == CICLO_BAJANDO) {
    g_estadoCiclo = CICLO_SUBIENDO;
    motorIniciarTramo(-PASOS_COMPRESION, DURACION_SUBIDA_MS);
    logMsg(LOG_DEBUG, "MOTOR", "Compresion completa (5cm), subiendo (recoil)");
  } else {
    g_estadoCiclo = CICLO_BAJANDO;
    motorIniciarTramo(PASOS_COMPRESION, DURACION_BAJADA_MS);
    logMsg(LOG_DEBUG, "MOTOR", "Recoil completo, bajando (siguiente compresion)");
  }
}

float motorPosicionCm() {
  return (float)g_posicionPasos / MOTOR_PASOS_POR_CM;
}
