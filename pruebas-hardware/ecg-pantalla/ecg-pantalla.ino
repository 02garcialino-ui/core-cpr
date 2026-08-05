// =====================================================
//  Prueba aislada de hardware real: ECG (AD8232 + ADS1115) + Laser de
//  profundidad (VL53L0X) + Pantalla Nextion. NO usa la maquina de
//  estados (todavia no hay PPG/fuerza/motor reales) -- ver
//  hmi_pantalla.h de esta carpeta.
//
//  Que hace:
//  - Lee el AD8232 (via ADS1115) y lo grafica en tiempo real en el
//    componente Waveform "s0" de la pagina "inicio" (misma pantalla
//    armada en T6.1/T6.3, no hace falta editar el proyecto Nextion).
//  - Muestra en "txtEstado" si hay o no hay senal cardiaca.
//  - Lee el VL53L0X y lo imprime por el puerto serial (no va a la
//    pantalla en esta prueba, solo se valida que el sensor responda).
// =====================================================

#include "config.h"
#include "logger.h"
#include "sensor_ecg.h"
#include "sensor_profundidad.h"
#include "hmi_pantalla.h"

bool habiaSenal = false;
bool primeraVuelta = true;

void setup() {
  loggerIniciar();
  logMsg(LOG_INFO, "MAIN", "Prueba aislada: ECG + Profundidad + Pantalla (hardware real)");

  hmiIniciar();
  ecgIniciar();
  profundidadIniciar();

  hmiMostrarTexto("Esperando lectura...");
}

void loop() {
  // La pantalla se refresca cada 40ms (25 veces por segundo): suficiente
  // para ver la forma de la onda sin saturar el puerto serial de la
  // Nextion (corre a 9600 baudios).
  static unsigned long ultimoChequeoMs = 0;
  if (millis() - ultimoChequeoMs < 40) {
    return;
  }
  ultimoChequeoMs = millis();

  float valorMv = ecgLeerValor();
  bool haySenal = ecgHaySenal();

  // Si el ADS1115 no responde (NAN), no mandamos ese punto a la grafica
  // -- ya quedo el error en el log (ver ecgHaySenal en sensor_ecg.cpp).
  if (!isnan(valorMv)) {
    hmiGraficarEcg(valorMv);
  }

  if (primeraVuelta || haySenal != habiaSenal) {
    if (haySenal) {
      hmiMostrarTexto("SENAL ECG DETECTADA");
      logMsg(LOG_INFO, "MAIN", ">>> Senal ECG detectada");
    } else {
      hmiMostrarTexto("SIN SENAL ECG");
      logMsg(LOG_WARN, "MAIN", ">>> Sin senal ECG (revisar electrodos)");
    }
    habiaSenal = haySenal;
    primeraVuelta = false;
  }

  // El VL53L0X se revisa mas lento (cada 300ms): no hace falta la
  // misma velocidad que el ECG, y asi la consola no se llena.
  static unsigned long ultimaProfundidadMs = 0;
  if (millis() - ultimaProfundidadMs >= 300) {
    ultimaProfundidadMs = millis();
    float cm = profundidadLeerCm();
    if (isnan(cm)) {
      logMsg(LOG_WARN, "MAIN", "Profundidad: sin lectura valida");
    } else {
      logMsg(LOG_INFO, "MAIN", "Profundidad: " + String(cm, 1) + " cm");
    }
  }
}
