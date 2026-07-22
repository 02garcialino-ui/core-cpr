#include "ads1115.h"
#include "config.h"
#include "logger.h"
#include <Wire.h>

// --- Registros internos del chip (vienen en su hoja de datos) ---
#define ADS1115_REG_CONVERSION  0x00   // aqui queda el resultado de la medicion
#define ADS1115_REG_CONFIG      0x01   // aqui se le dice que y como medir

// Con la ganancia +-4.096V, cada "paso" del contador vale 0.125 mV.
// Sale de: 4096 mV / 32768 pasos = 0.125 mV por paso.
#define ADS1115_MV_POR_PASO     0.125

void ads1115Iniciar() {
  Wire.begin();
  if (ads1115Presente()) {
    logMsg(LOG_INFO, "ADS1115", "Chip detectado en 0x48");
  } else {
    logMsg(LOG_WARN, "ADS1115", "Chip NO detectado (normal en simulacion)");
  }
}

bool ads1115Presente() {
  Wire.beginTransmission(ADS1115_DIR_I2C);
  return (Wire.endTransmission() == 0);   // 0 = el chip contesto
}

float ads1115LeerCanal(uint8_t canal) {
  if (canal > 3) {
    logMsg(LOG_ERROR, "ADS1115", "Canal invalido: " + String(canal));
    return NAN;
  }

  // Armamos la orden de medicion (16 bits).
  // 0xC383 = medir una sola vez, canal 0 contra tierra, rango +-4.096V.
  // Los 3 bits del canal se corren de lugar segun cual queremos leer.
  uint16_t orden = 0xC383 | (canal << 12);

  Wire.beginTransmission(ADS1115_DIR_I2C);
  Wire.write(ADS1115_REG_CONFIG);
  Wire.write(orden >> 8);      // primero la mitad alta
  Wire.write(orden & 0xFF);    // luego la mitad baja
  if (Wire.endTransmission() != 0) {
    logMsg(LOG_ERROR, "ADS1115", "No responde al pedir medicion");
    return NAN;
  }

  delay(9);   // el chip tarda ~8 ms en medir a 128 muestras/segundo

  // Ahora leemos el resultado.
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
