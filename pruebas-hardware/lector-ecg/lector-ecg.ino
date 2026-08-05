// =====================================================
//  Lector ECG -- diagnostico de hardware real (AD8232 + ADS1115)
//
//  Que hace: lee el canal 0 del ADS1115 (donde entra el OUTPUT del
//  AD8232) cada 500ms y lo imprime en mV. A diferencia del escaner
//  I2C (que solo confirma "hay algo conectado"), esto SI lee el
//  valor real -- sirve para ver si la lectura cambia al tocar los
//  electrodos/broches del AD8232 con el dedo.
//
//  Conexion:
//    AD8232 OUTPUT -> ADS1115 A0 (canal 0)
//    AD8232 3.3V/GND -> mismo riel que el resto
//    AD8232 LO+/LO-  -> sin conectar por ahora (deteccion de
//                        electrodo suelto, se suma mas adelante)
//    ADS1115 -> igual que en el escaner I2C (SDA=21, SCL=22, ADDR=GND)
//
//  Que esperar en la terminal: un numero que se mueve un poco solo
//  (ruido normal), y que cambia mas fuerte al tocar con el dedo los
//  broches RA y LA del AD8232 (no hace falta tener electrodos reales
//  pegados todavia para esta primera prueba).
// =====================================================

#include <Wire.h>

#define ADS1115_DIR_I2C   0x48   // direccion I2C del ADS1115 (ADDR a GND)
#define ADS1115_CANAL_ECG 0      // canal donde entra el OUTPUT del AD8232 (A0)

// Con la ganancia +-4.096V, cada "paso" del contador vale 0.125 mV.
// Sale de: 4096 mV / 32768 pasos = 0.125 mV por paso.
#define ADS1115_MV_POR_PASO 0.125

#define ADS1115_REG_CONVERSION 0x00
#define ADS1115_REG_CONFIG     0x01

float ads1115LeerCanal(uint8_t canal) {
  // Armamos la orden de medicion (16 bits): medir una sola vez, el
  // canal indicado contra tierra, rango +-4.096V.
  uint16_t orden = 0xC383 | (canal << 12);

  Wire.beginTransmission(ADS1115_DIR_I2C);
  Wire.write(ADS1115_REG_CONFIG);
  Wire.write(orden >> 8);
  Wire.write(orden & 0xFF);
  if (Wire.endTransmission() != 0) {
    return NAN;   // el chip no contesto
  }

  delay(9);   // el chip tarda ~8ms en medir a 128 muestras/segundo

  Wire.beginTransmission(ADS1115_DIR_I2C);
  Wire.write(ADS1115_REG_CONVERSION);
  if (Wire.endTransmission() != 0) {
    return NAN;
  }

  if (Wire.requestFrom((uint8_t)ADS1115_DIR_I2C, (uint8_t)2) != 2) {
    return NAN;
  }

  int16_t crudo = (Wire.read() << 8) | Wire.read();
  return crudo * ADS1115_MV_POR_PASO;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // espera a que la consola serial este lista
  }

  Wire.begin();   // SDA=21, SCL=22 por defecto en la ESP32

  Serial.println();
  Serial.println("=== Lector ECG listo ===");
  Serial.println("Toca los broches RA/LA del AD8232 con el dedo para ver la lectura cambiar.");
}

void loop() {
  float valorMv = ads1115LeerCanal(ADS1115_CANAL_ECG);

  if (isnan(valorMv)) {
    Serial.println("ECG: sin lectura (el ADS1115 no contesto)");
  } else {
    Serial.print("ECG: ");
    Serial.print(valorMv, 1);
    Serial.println(" mV");
  }

  delay(500);
}
