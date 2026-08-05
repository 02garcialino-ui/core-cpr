#ifndef CONFIG_H
#define CONFIG_H

// =====================================================
//  Prueba aislada de hardware real: ECG (AD8232 + ADS1115) + Pantalla
//  (Nextion). Copia recortada de firmware-esp32/config.h, con SOLO
//  los valores que este sketch necesita. NO modifica ni reemplaza el
//  proyecto principal.
// =====================================================

// A diferencia del proyecto principal, aqui SIEMPRE es hardware real:
// no existe el modo simulado en esta prueba.
#define MODO_SIMULACION  false
#define MODO_DEBUG       true    // true = imprime logs de nivel DEBUG

#define SERIAL_BAUDIOS   115200  // velocidad de la consola serial

// ---- ECG real (AD8232 -> ADS1115) ----
#define ADS1115_DIR_I2C   0x48   // direccion I2C del ADS1115 (ADDR a GND)
#define ADS1115_CANAL_ECG 0      // canal donde entra la salida del AD8232 (A0)

// Por debajo de este valor consideramos que NO hay senal cardiaca.
// OJO: PROVISIONAL, igual que en el proyecto principal -- se recalibra
// mirando los valores reales que entrega tu AD8232 (con y sin electrodos).
#define UMBRAL_ECG_MV    1500    // milivolts

// ---- Pantalla Nextion ----
#define PIN_NEXTION_RX   13     // ESP32 recibe (se conecta al TX de la Nextion)
#define PIN_NEXTION_TX   14     // ESP32 transmite (se conecta al RX de la Nextion)
#define NEXTION_BAUDIOS  9600   // velocidad de fabrica de la Nextion

#endif
