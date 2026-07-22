#ifndef ADS1115_H
#define ADS1115_H

#include <Arduino.h>

// =====================================================
//  Driver del ADS1115 (convertidor analogico-digital externo)
//
//  Por que existe este archivo separado:
//  El AD8232 entrega una senal analogica muy pequena. El ADC interno
//  de la ESP32 es ruidoso y poco lineal, asi que usamos un ADC externo
//  de 16 bits. Aislarlo en su propio modulo permite cambiarlo despues
//  sin tocar la logica del ECG.
//
//  === NO VALIDADO EN SIMULACION ===
//  Wokwi no incluye el ADS1115, asi que este codigo NO se pudo probar.
//  Se valida cuando llegue el chip fisico (tarea T9.4 del plan).
// =====================================================

// Arranca el bus I2C. Llamar una vez en setup().
void ads1115Iniciar();

// Lee un canal (0 a 3) y devuelve el valor en milivolts.
// Devuelve NAN si el chip no responde.
float ads1115LeerCanal(uint8_t canal);

// True si el chip contesta en el bus I2C.
bool ads1115Presente();

#endif
