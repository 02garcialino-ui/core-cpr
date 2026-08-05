// =====================================================
//  Lector Laser -- diagnostico de hardware real (VL53L0X)
//  (mismo codigo que lector-laser.ino, version PlatformIO)
//
//  Conexion: VIN->3.3V, GND->GND, SCL->GPIO22, SDA->GPIO21,
//  XSHUT y GPIO1 sin usar.
// =====================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
bool sensorOk = false;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  Wire.begin();
  sensorOk = lox.begin();

  if (sensorOk) {
    Serial.println();
    Serial.println("=== Lector Laser listo ===");
    Serial.println("Acerca y aleja la mano del sensor para ver la distancia cambiar.");
  } else {
    Serial.println();
    Serial.println("VL53L0X no responde. Revisar cableado I2C.");
  }
}

void loop() {
  if (!sensorOk) {
    Serial.println("LASER: sensor no disponible");
    delay(1000);
    return;
  }

  VL53L0X_RangingMeasurementData_t medicion;
  lox.rangingTest(&medicion, false);

  if (medicion.RangeStatus == 4) {
    Serial.println("LASER: fuera de rango (nada enfrente)");
  } else {
    Serial.print("LASER: ");
    Serial.print(medicion.RangeMilliMeter / 10.0, 1);
    Serial.println(" cm");
  }

  delay(300);
}
