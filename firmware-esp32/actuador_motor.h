#ifndef ACTUADOR_MOTOR_H
#define ACTUADOR_MOTOR_H

#include <Arduino.h>

// =====================================================
//  Modulo del motor (NEMA 23 + driver STEP/DIR/ENABLE)
//
//  En Wokwi se simula con un driver A4988 + motor generico (mismo
//  protocolo STEP/DIR/ENABLE que el HBS57 real). Cambiar de driver es
//  solo cableado; este codigo no cambia.
//
//  El movimiento es NO bloqueante: motorIniciarTramo() arranca un
//  movimiento y motorActualizarTramo() hay que llamarla en cada vuelta
//  de loop() para que avance. Asi el resto del programa (lecturas de
//  fuerza/profundidad) sigue funcionando mientras el motor se mueve.
// =====================================================

// Configura los pines. Llamar una vez en setup().
void motorIniciar();

// Habilita (true) o deshabilita (false) el driver.
void motorHabilitar(bool activar);

// Busca la posicion "cero" (extremo retraido) moviendo el motor hacia el
// final de carrera de home. Se detiene ahi al encontrarlo. Esta funcion
// SI bloquea (el homing es un evento unico al arrancar, no repetitivo).
// Devuelve true si lo encontro, false si se paso del limite de seguridad
// sin activarlo (posible switch desconectado o roto).
bool motorHoming();

// Inicia un movimiento suave (con rampa de aceleracion/desaceleracion) de
// 'pasos' pulsos (signo = direccion) repartido en 'duracionMs'
// milisegundos. NO bloquea: hay que llamar motorActualizarTramo() en
// cada vuelta del loop() hasta que devuelva false.
void motorIniciarTramo(long pasos, unsigned long duracionMs);

// Avanza el movimiento en curso un paso, si ya corresponde segun el
// tiempo transcurrido. Devuelve true mientras el tramo sigue en marcha,
// false cuando termino. Llamar en cada vuelta de loop().
bool motorActualizarTramo();

// Posicion actual del piston en cm, contando pasos desde el home (0 = home).
// Se resetea a 0 cuando motorHoming() encuentra el switch. Es un conteo
// interno (una suposicion, no un hecho verificado) -- para eso esta el
// cruce con el sensor de profundidad real (VL53L0X, T5.4).
float motorPosicionCm();

// ---- Ciclo de compresion (T5.3, conectado a la FSM en T8.1) ----
// Movimiento suave a RITMO_CPM_OBJETIVO, alternando bajada/subida (recoil).

// Arranca el ciclo desde cero (bajando). Llamar cuando la FSM entra a
// FSM_COMPRIMIENDO.
void motorCicloIniciar();

// Avanza el ciclo un paso, si ya corresponde. Llamar en cada vuelta de
// loop() mientras motorCicloEnMovimiento() sea true.
void motorCicloActualizar();

// Pide detener el ciclo. Si el piston va bajando, corta ese tramo y arma
// uno nuevo de subida DESDE LA POSICION ACTUAL (no desde el fondo), al
// mismo ritmo que una subida normal -- nunca sigue empujando ni queda
// trabado contra el pecho. Si ya iba subiendo (recoil), lo deja terminar
// ese tramo. Llamar cuando la FSM sale de FSM_COMPRIMIENDO (paciente
// revivio). No se usa para el paro de emergencia ni el aborto por
// limite (T7.1/T7.2): esos cortan con motorHabilitar(false) al instante,
// a proposito.
void motorCicloDetener();

// true mientras el ciclo tiene algo pendiente (comprimiendo o
// terminando de retraerse tras motorCicloDetener()). false = motor
// quieto, no hace falta llamar mas a motorCicloActualizar().
bool motorCicloEnMovimiento();

#endif
