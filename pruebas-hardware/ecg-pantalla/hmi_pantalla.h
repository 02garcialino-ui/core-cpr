#ifndef HMI_PANTALLA_H
#define HMI_PANTALLA_H

#include <Arduino.h>

// =====================================================
//  Version RECORTADA de hmi_pantalla, solo para esta prueba aislada.
//
//  El original (firmware-esp32/hmi_pantalla.h) tiene funciones que
//  dependen de la maquina de estados completa (hmiMostrarModo,
//  hmiMostrarEstado con la pagina de confirmacion, hmiLeerToque).
//  Como esta prueba NO corre la FSM (todavia no hay PPG/fuerza/motor
//  reales), esas funciones no aplican aqui.
//
//  Esta version solo tiene lo que se necesita para ver:
//  - la grafica del ECG en tiempo real (componente Waveform "s0")
//  - un mensaje simple de si hay o no hay senal (componente "txtEstado")
//  ambos en la pagina "inicio" del proyecto Nextion que ya armaste en
//  T6.1/T6.3 -- no hace falta crear componentes nuevos en el editor.
// =====================================================

// Prepara el puerto serial hacia la pantalla y muestra la pagina inicial.
void hmiIniciar();

// Agrega un punto a la grafica del ECG (componente Waveform "s0").
// Igual que en el proyecto principal: recibe el valor en milivolts
// (0-3300) y lo reescala a 0-255 antes de mandarlo.
void hmiGraficarEcg(float valorMv);

// Actualiza el texto de "txtEstado" con un mensaje simple.
void hmiMostrarTexto(const String &texto);

#endif
