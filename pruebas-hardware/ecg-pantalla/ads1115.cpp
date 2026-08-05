#include "ads1115.h"
#include "config.h"
#include "logger.h"
#include <Wire.h>

// Copia sin cambios de firmware-esp32/ads1115.cpp.

#define ADS1115_REG_CONVERSION  0x00
#define ADS1115_REG_CONFIG      0x01
#define ADS1115_MV_POR_PASO     0.125

void ads1115Iniciar() {
  Wire.begin();
  if (ads1115Presente()) {
    logMsg(LOG_INFO, "ADS1115", "Chip detectado en 0x48");
  } else {
    logMsg(LOG_WARN, "ADS1115", "Chip NO detectado (revisar cableado I2C)");
  }
}

bool ads1115Presente() {
  Wire.beginTransmission(ADS1115_DIR_I2C);
  return (Wire.endTransmission() == 0);
}

float ads1115LeerCanal(uint8_t canal) {
  if (canal > 3) {
    logMsg(LOG_ERROR, "ADS1115", "Canal invalido: " + String(canal));
    return NAN;
  }

  uint16_t orden = 0xC383 | (canal << 12);

  Wire.beginTransmission(ADS1115_DIR_I2C);
  Wire.write(ADS1115_REG_CONFIG);
  Wire.write(orden >> 8);
  Wire.write(orden & 0xFF);
  if (Wire.endTransmission() != 0) {
    logMsg(LOG_ERROR, "ADS1115", "No responde al pedir medicion");
    return NAN;
  }

  delay(9);

  Wire.beginTransmission(ADS1115_DIR_I2C);
  Wire.write(ADS1115_REG_CONVERSION);
  if (Wire.endTransmission() != 0) {
    logMsg(LOG_ERROR, "ADS1115", "No responde al leer resultado");
    return NAN;
  }

  if (Wire.requestFrom((uint8_t)ADS1115_DIR_I2C, (uint8_t)2) != 2) {
    logMsg(LOG_ERROR, "ADS1115", "Lectura incompleta");
    return NAN;
  }

  int16_t crudo = (Wire.read() << 8) | Wire.read();
  return crudo * ADS1115_MV_POR_PASO;
}
