#ifndef ADS1115_H
#define ADS1115_H

#include <Arduino.h>

// Copia sin cambios de firmware-esp32/ads1115.h. Aqui SI se valida con
// el chip fisico -- ese es el objetivo de esta prueba.

void ads1115Iniciar();
float ads1115LeerCanal(uint8_t canal);
bool ads1115Presente();

#endif
