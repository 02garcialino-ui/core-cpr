#include "sensor_ppg.h"
#include "config.h"
#include "logger.h"
#include "simulador_falla.h"

// El modo real necesita la libreria "SparkFun MAX3010x Pulse and
// Proximity Sensor Library" (Arduino IDE: Sketch > Include Library >
// Manage Libraries > buscar "SparkFun MAX3010x"). Su clase se llama
// MAX30105 aunque tambien sirve para el MAX30102 (misma familia de
// chips, mismo protocolo).
#if !MODO_SIMULACION
#include <Wire.h>
#include <MAX30105.h>

static MAX30105 g_particleSensor;
static bool g_ppgOk = false;   // false hasta que begin() confirme el chip
#endif

void ppgIniciar() {
  if (MODO_SIMULACION) {
    pinMode(PIN_PPG_SIM, INPUT);
    logMsg(LOG_INFO, "PPG", "Modo SIMULADO: leyendo potenciometro en GPIO " +
                            String(PIN_PPG_SIM));
    // === reemplazar por sensor real cuando llegue ===
  } else {
#if !MODO_SIMULACION
    Wire.begin();   // SDA=21, SCL=22 (idempotente si ya lo llamo el ECG)
    g_ppgOk = g_particleSensor.begin(Wire, I2C_SPEED_FAST);

    if (g_ppgOk) {
      // Brillo medio, 4 muestras promediadas, LEDs Rojo+IR (el minimo
      // que necesita esta libreria para tener el canal IR disponible,
      // que es el que usamos), 100 muestras/seg, pulso de 411us, rango
      // del ADC 4096. Valores de arranque tipicos del ejemplo oficial
      // de SparkFun -- PROVISIONAL, se ajustan si hace falta (T9.4).
      g_particleSensor.setup(0x1F, 4, 2, 100, 411, 4096);
      logMsg(LOG_INFO, "PPG", "Modo REAL: MAX30102 detectado y configurado");
    } else {
      logMsg(LOG_ERROR, "PPG", "Modo REAL: MAX30102 no responde (revisar cableado I2C)");
    }
#endif
  }
}

// ESTA es la unica funcion que cambia entre simulado y real.
// Todo lo demas del proyecto funciona igual en los dos casos.
float ppgLeerValor() {
  if (MODO_SIMULACION) {
    if (simuladorFallaActiva()) {
      return NAN;   // === SOLO PARA PRUEBAS (T7.2) ===
    }

    // El ADC de la ESP32 entrega 0..4095. Lo escalamos a 0..3300 mV
    // para que el numero se parezca a lo que dara el sensor real.
    int lectura = analogRead(PIN_PPG_SIM);
    return (lectura / 4095.0) * 3300.0;
    // === reemplazar por sensor real cuando llegue ===
  } else {
#if !MODO_SIMULACION
    if (!g_ppgOk) {
      return NAN;   // el chip nunca respondio en ppgIniciar()
    }
    return (float)g_particleSensor.getIR();
#else
    return NAN;
#endif
  }
}

// La decision. Esta logica NO cambia al pasar a hardware real:
// solo cambia de donde salio el numero, no como se interpreta.
bool ppgHayPulso() {
  float valor = ppgLeerValor();

  // NAN significa que el sensor fallo. No es lo mismo que "no hay pulso":
  // un paciente con pulso pero con el sensor roto no debe tratarse como paro.
  // Aqui sigue devolviendo false (tratado como "sin pulso"): quien decide
  // si hay que preguntar antes de asumir el paro es la FSM, usando
  // ppgSensorFallo() para distinguir el caso (T7.2 Parte B).
  if (isnan(valor)) {
    logMsg(LOG_ERROR, "PPG", "Lectura invalida: el sensor no responde");
    return false;
  }

  // El umbral es distinto segun el modo, porque la unidad tambien lo es
  // (ver la nota en sensor_ppg.h y en config.h junto a UMBRAL_PPG_IR).
  float umbral = MODO_SIMULACION ? UMBRAL_PPG_MV : UMBRAL_PPG_IR;
  bool hayPulso = (valor > umbral);

  if (MODO_DEBUG) {
    logMsg(LOG_DEBUG, "PPG", "valor=" + String(valor, 1) +
                             (MODO_SIMULACION ? "mV" : " (IR)") +
                             "  umbral=" + String(umbral, 1) +
                             "  -> hayPulso=" + (hayPulso ? "SI" : "NO"));
  }

  return hayPulso;
}

bool ppgSensorFallo() {
  return isnan(ppgLeerValor());
}
