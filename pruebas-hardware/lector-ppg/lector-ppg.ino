// =====================================================
//  Lector PPG -- diagnostico de hardware real (MAX30102)
//
//  Lee el canal infrarrojo del MAX30102 cada 300ms y lo imprime.
//  Poné el dedo sobre el sensor: el numero deberia subir fuerte
//  (el dedo bloquea la luz ambiente y refleja la del propio sensor).
//
//  Necesita la libreria "SparkFun MAX3010x Pulse and Proximity
//  Sensor Library" (Arduino IDE: Sketch > Include Library > Manage
//  Libraries > buscar "SparkFun MAX3010x").
//
//  Conexion: VIN->3.3V, GND->GND, SCL->GPIO22, SDA->GPIO21, INT sin usar.
// =====================================================

#include <Wire.h>
#include <MAX30105.h>

MAX30105 particleSensor;
bool sensorOk = false;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  Wire.begin();
  // I2C_SPEED_STANDARD (100kHz) en vez de I2C_SPEED_FAST (400kHz): con
  // jumpers sueltos (sin soldar) la velocidad rapida es mas sensible a
  // una conexion un poco floja -- el escaner I2C, que si detecto este
  // chip, uso la velocidad normal por defecto.
  sensorOk = particleSensor.begin(Wire, I2C_SPEED_STANDARD);

  if (sensorOk) {
    // Brillo medio, 4 muestras promediadas, LEDs Rojo+IR, 100
    // muestras/seg, pulso de 411us, rango del ADC 4096.
    particleSensor.setup(0x1F, 4, 2, 100, 411, 4096);
    Serial.println();
    Serial.println("=== Lector PPG listo ===");
    Serial.println("Pon el dedo sobre el sensor para ver el valor subir.");
  } else {
    Serial.println();
    Serial.println("MAX30102 no responde. Revisar cableado I2C.");
  }
}

void loop() {
  if (!sensorOk) {
    Serial.println("PPG: sensor no disponible");
    delay(1000);
    return;
  }

  long irValue = particleSensor.getIR();
  Serial.print("PPG (IR): ");
  Serial.println(irValue);

  delay(300);
}
