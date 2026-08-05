# Diseño eléctrico — Conexiones y alimentación

> Cómo se conecta cada sensor, actuador y fuente a la ESP32. Incluye el cableado ya confirmado en simulación (Wokwi) y el plan propuesto para hardware real (T9.4). Documento vivo — ver nota al final.

## Nota honesta

Este plano tiene dos capas: lo **confirmado** (la simulación de Wokwi, que corre tal cual está en `firmware-esp32/diagram.json`) y lo **propuesto** para el hardware real, que todavía no se probó. Revisando `config.h` aparecieron **dos conflictos de pines sin resolver**:

- **GPIO 21** — hoy lo usa el botón "Reintentar lectura", pero también es el **SDA** por defecto del bus I2C que van a compartir el ADS1115, el MAX30102 y el VL53L0X reales.
- **GPIO 22** — el comentario de T7.3 en `config.h` planea reasignar el buzzer acá, pero también es el **SCL** por defecto del mismo bus I2C.

Más abajo hay una propuesta de solución para los dos, marcada como propuesta — no un cambio ya aplicado en el código.

## Alimentación

Dos fuentes, **sin GND compartido** (aislamiento galvánico entre potencia y lógica):

```
[FUENTE 24V, 5–6A] → [FUSIBLE 10A] → [INTERRUPTOR ON/OFF] → [DRIVER HBS57] → [MOTOR NEMA23 + encoder]

┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄ sin GND compartido — aislamiento galvánico ┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄

[FUENTE 5V, 2–3A] → [RIEL DE LÓGICA (GND común)] → alimenta:
                                                      · ESP32 (5V/VIN)
                                                      · Buffer 74AHCT125
                                                      · Sensores I2C + HX711
                                                      · Pantalla Nextion
                                                      · Buzzer piezo activo
```

Si algo falla del lado del motor (24V), no puede dañar la electrónica de control (5V). El buffer 74AHCT125 y el aislamiento óptico interno del driver HBS57 son los únicos puntos donde ambos mundos se "tocan", sin compartir tierra.

## Sensores y actuadores — conexión a la ESP32

```
   SENSORES / ENTRADAS                          ESP32 DEVKITC                   ACTUADORES / SALIDAS
                                            (↑ alimentado en 5V/3V3, ver arriba)

  BUS I2C ─────────────── GPIO21 SDA ⚠ ────┤                              ├──── GPIO25 STEP ───┐
   ADS1115 (ECG)          GPIO22 SCL ⚠ ────┤                              ├──── GPIO26 DIR ────┼── MOTOR
   MAX30102 (PPG)                          │                              ├──── GPIO27 ENABLE ─┘   (buffer 74AHCT125
   VL53L0X (profundidad)                   │                              │                          → driver HBS57
                                            │                              │                          → NEMA23+encoder)
  HX711 ────────────────── GPIO16 DT ──────┤                              │
   (celda de carga)        GPIO17 SCK ─────┤                              ├──── GPIO13 RX ──────┐
                                            │                              ├──── GPIO14 TX ──────┴── PANTALLA NEXTION
  SELECTOR DE MODO ──────── GPIO19 ────────┤                              │                          (Serial2, 9600 baud)
                                            │                              ├──── GPIO0 ──────────────  BUZZER (P)
  PARO DE EMERGENCIA ────── GPIO15 ────────┤                              │
                                            │                              ├──── GPIO 2·4·5·18·23 ──── LEDs DIAGNÓSTICO
  FIN DE CARRERA (HOME) ─── GPIO33 ────────┤                              │        (×5, debug)
                                            │
  BOTÓN "REINTENTAR" (P) ── GPIO32 ────────┤
                                            │
  FIN DE CARRERA TOPE (P) ─ GPIO35 ────────┤

     ⚠ = conflicto de pin, sin resolver          (P) = propuesta de este documento
```

> Diagrama a color con la misma información (SVG interactivo): ver el plano publicado — [Core CPR — Plano eléctrico y de conexiones](https://claude.ai/code/artifact/90f7b2d2-1abc-46d8-ac8b-a01e7356e9e7) (enlace privado del usuario, no público).

## Tabla completa de pines

| Pin | Simulación (Wokwi, confirmado) | Hardware real (plan) | Nota |
|---|---|---|---|
| GPIO 0 | buzzer1 | Buzzer **PROP** | se mantiene; probar arranque con buzzer real conectado (pin de boot-strap) |
| GPIO 2 | LED estado (ECG) | LED diagnóstico | indicador de prueba, no imprescindible en el equipo final |
| GPIO 4 | LED PPG | LED diagnóstico | ídem |
| GPIO 5 | LED fuerza | LED diagnóstico | ídem |
| GPIO 12 | libre | libre (reserva) | strapping pin — evitar salvo necesidad real |
| GPIO 13 | — (Wokwi no simula la Nextion) | Nextion RX | ESP32 recibe ← TX de la pantalla |
| GPIO 14 | — (ídem) | Nextion TX | ESP32 transmite → RX de la pantalla |
| GPIO 15 | sw_paro_emergencia | Paro de emergencia (NC) | HIGH = activado (presionado o cable cortado) |
| GPIO 16 | hx711_1 DT | HX711 DT | sin cambios |
| GPIO 17 | hx711_1 SCK | HX711 SCK | sin cambios |
| GPIO 18 | LED profundidad | LED diagnóstico | — |
| GPIO 19 | sw_modo | Selector de modo | GND = niño, INPUT_PULLUP |
| GPIO 21 | boton_reintentar | Bus I2C — SDA **⚠ CONFLICTO** | hoy vive acá el botón reintentar; ver propuesta en GPIO 32 |
| GPIO 22 | sw_sim_falla (solo prueba) | Bus I2C — SCL **⚠ CONFLICTO** | el plan viejo de T7.3 ponía el buzzer acá; se propone no moverlo (queda en GPIO 0) |
| GPIO 23 | LED compresión | LED diagnóstico | — |
| GPIO 25 | driver_motor STEP | Motor STEP | vía buffer 74AHCT125 |
| GPIO 26 | driver_motor DIR | Motor DIR | vía buffer 74AHCT125 |
| GPIO 27 | driver_motor EN | Motor ENABLE | activo en LOW — confirmar con datasheet del HBS57 real |
| GPIO 32 | pot3 (profundidad simulada) | Botón "Reintentar" **PROP** | libera el GPIO 21 para el bus I2C |
| GPIO 33 | switch_home | Fin de carrera — home | sin cambios |
| GPIO 34 | pot1 (ECG simulado) | libre, sin uso asignado | input-only, sin pull-up interno |
| GPIO 35 | pot2 (PPG simulado) | Fin de carrera — tope **PROP** | input-only; necesita resistencia pull-up externa ≈10 kΩ (T7.4) |

## Por qué está armado así

**Dos fuentes, sin GND compartido.** El lado de potencia (24V, motor) queda aislado del lado de lógica (5V, ESP32 y sensores) a propósito: si algo falla del lado del motor, no puede dañar la electrónica de control.

**Un solo bus I2C para tres sensores.** ADS1115 (ECG), MAX30102 (PPG) y VL53L0X (profundidad) son los tres chips I2C — comparten las mismas dos líneas (SDA/SCL) en vez de gastar un GPIO cada uno. Misma razón por la que el análisis de presupuesto de pines concluyó que no hace falta comprar un expansor I2C.

**Por qué el botón "Reintentar" se propone mover a GPIO 32.** GPIO 21 es el SDA por defecto: en simulación no molesta (no hay bus I2C real todavía), pero en hardware real chocaría con los tres sensores. GPIO 32 queda libre justo cuando se saca el potenciómetro de profundidad simulado, y es un pin normal con pull-up interno — no exige cambiar más que el número en `config.h`.

**Por qué el buzzer NO se mueve a GPIO 22.** El plan anotado en T7.3 lo movía ahí, pero GPIO 22 es el SCL por defecto. Mover el buzzer a un tercer pin nuevo (ej. GPIO 12) suma otro "strapping pin" a evitar; más simple es dejarlo en GPIO 0 tanto en simulación como en hardware real, y probar el arranque con el buzzer real conectado.

## Este diseño está sujeto a cambios

Todo lo marcado **PROP** (propuesta) y **⚠ CONFLICTO** en este documento es un plan de papel, no algo probado con hardware. Cuando lleguemos a **T9.3/T9.4** (conectar los sensores, el driver y la pantalla al circuito definitivo), es muy probable que algo de esto cambie: un pin que no responda como se espera, un sensor que necesite otro cableado, un driver real con terminales distintas al genérico usado acá, etc. Cuando eso pase, te ayudo a ajustar este documento (y el código en `config.h`) para que quede reflejando lo que realmente terminó funcionando — no hace falta rehacerlo de cero, se corrige sobre esta base.

## Próximo paso

Antes de cablear en T9.3, decidir los dos conflictos marcados **⚠ CONFLICTO** (GPIO 21 y GPIO 22) — la propuesta de este documento es una base para esa conversación, no la decisión final.
