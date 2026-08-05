#include "sensor_profundidad.h"
#include "logger.h"
#include <Wire.h>
#include <VL53L0X.h>

static VL53L0X sensor;
static bool sensorOk = false;

void profundidadIniciar() {
  sensor.setTimeout(500);

  if (!sensor.init()) {
    logMsg(LOG_ERROR, "PROFUNDIDAD", "VL53L0X no responde (revisar cableado I2C)");
    sensorOk = false;
    return;
  }

  sensor.startContinuous();
  sensorOk = true;
  logMsg(LOG_INFO, "PROFUNDIDAD", "VL53L0X listo, lectura continua");
}

float profundidadLeerCm() {
  if (!sensorOk) {
    return NAN;
  }

  uint16_t mm = sensor.readRangeContinuousMillimeters();

  if (sensor.timeoutOccurred()) {
    logMsg(LOG_ERROR, "PROFUNDIDAD", "Timeout leyendo el VL53L0X");
    return NAN;
  }

  return mm / 10.0;
}
