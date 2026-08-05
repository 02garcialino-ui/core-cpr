// =====================================================
//  Escaner I2C -- diagnostico de hardware real
//
//  Que hace: recorre TODAS las direcciones I2C posibles (1 a 127) y
//  avisa cuales contestan. No sabe nada de sensores especificos (a
//  diferencia de pruebas-hardware/ecg-pantalla, que ya asume que sabe
//  que hay en cada direccion) -- sirve para confirmar que el cableado
//  esta bien ANTES de escribir codigo para leer cada sensor de verdad.
//
//  Por que existe: en la sesion anterior, ni el ADS1115 ni el VL53L0X
//  aparecian en el bus (ver PLAN-PROYECTO.md, seccion "Pendiente --
//  bloqueo en prueba de hardware ECG + Pantalla"). Antes de sumar un
//  tercer chip I2C (MAX30102) al mismo bus, conviene resolver eso.
//
//  Direcciones esperadas (segun datasheet de cada chip, confirmar con
//  lo que este sketch realmente encuentre):
//    - ADS1115  -> 0x48 (con el pin ADDR conectado a GND, ver config.h)
//    - MAX30102 -> 0x57
//    - VL53L0X  -> 0x29
//
//  Conexion: SDA -> GPIO 21, SCL -> GPIO 22 (pines por defecto de la
//  ESP32, reservados para I2C -- ver la nota en firmware-esp32/config.h).
//  Los tres sensores van en PARALELO al mismo par de cables SDA/SCL,
//  mas alimentacion (3.3V) y GND compartidos.
// =====================================================

#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // espera a que la consola serial este lista (algunas placas la necesitan)
  }

  Wire.begin();   // SDA=21, SCL=22 por defecto en la ESP32

  Serial.println();
  Serial.println("=== Escaner I2C listo ===");
  Serial.println("Buscando dispositivos cada 5 segundos...");
}

void loop() {
  Serial.println();
  Serial.println("--- Nueva busqueda ---");

  int encontrados = 0;

  for (uint8_t direccion = 1; direccion < 127; direccion++) {
    // Intentamos "tocar la puerta" de cada direccion. Si el chip esta
    // ahi, contesta (error == 0). Si no hay nada conectado a esa
    // direccion, no contesta (error != 0) -- es normal, no es un fallo.
    Wire.beginTransmission(direccion);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      encontrados++;
      Serial.print("Dispositivo encontrado en 0x");
      if (direccion < 16) {
        Serial.print("0");   // para que siempre se vean 2 digitos, ej. 0x08
      }
      Serial.println(direccion, HEX);
    }
  }

  if (encontrados == 0) {
    Serial.println("No se encontro ningun dispositivo. Revisar:");
    Serial.println("  - SDA en GPIO 21 y SCL en GPIO 22 (no al reves)");
    Serial.println("  - Alimentacion (3.3V, NO 5V) y GND compartidos");
    Serial.println("  - Continuidad de los cables (con multimetro)");
  } else {
    Serial.print(encontrados);
    Serial.println(encontrados == 1 ? " dispositivo encontrado." : " dispositivos encontrados.");
  }

  delay(5000);   // repite cada 5s, para poder mover cables sin resubir el codigo
}
