# PLAN DE PROYECTO — Core CPR

> Dispositivo de RCP automatizada (prototipo académico, **solo modo niño funcional**).
> Documento vivo: lo vamos marcando a medida que avanzamos.

---

## 🎯 Objetivo del proyecto

Construir un dispositivo que, colocado sobre un **maniquí de RCP de niño**, detecte un paro cardíaco (mediante ECG + PPG) y administre **compresiones torácicas mecánicas** según el estándar AHA pediátrico: **100–120 compresiones por minuto, ~5 cm de profundidad, ~250 N máximo de fuerza**.

**Cómo se prueba:** el **PPG (pulso)** es quien decide si hay actividad vital, tanto para empezar como para detener las compresiones (así se cubre el caso clínico de Actividad Eléctrica Sin Pulso, AESP: el corazón puede tener actividad eléctrica sin bombear sangre). El ECG se sigue registrando en los logs, pero no decide.

En la prueba se combinan dos controles: un **Arduino** genera una señal de ECG simulada, con un switch para cortarla/reanudarla; y el sensor PPG se controla poniendo/retirando un dedo real sobre él (un sensor óptico de pulso no se puede simular con un potenciómetro, necesita tejido con sangre real). Tres escenarios:
- **Normal**: ECG activo + dedo puesto → vivo, no comprime.
- **Paro clásico**: ECG cortado + dedo retirado → comprime.
- **AESP (caso especial)**: ECG activo + dedo retirado → comprime igual, porque manda el PPG.

---

## 📖 Cómo usar este plan

### Estados de cada tarea
- `[ ]` **Pendiente** — todavía no la hicimos.
- `[x]` **Lograda** — funciona.
- `[-]` **No se pudo en este simulador** — probamos en el otro.
- `[~]` **En stand-by** — no salió en ninguno; buscamos alternativa (ej. Proteus).

### Quién hace cada tarea
- 🤖 **Claude** — código, cálculos, explicaciones, diagramas.
- 🧑 **Vos** — acciones físicas: comprar, cablear, medir, correr el simulador y reportar.
- 🤝 **Ambos** — trabajo conjunto.

### Regla de oro
- Avanzamos **de a poco** y **no pasamos a la fase siguiente** hasta cerrar la anterior.
- Claude **no edita/crea archivos sin tu confirmación** explícita.
- Al inicio de cada sesión, Claude lee este plan y te dice **la próxima tarea pendiente**.

### Simuladores
- **Wokwi** (principal — simula la ESP32; el código se transfiere igual al hardware).
- **Tinkercad** (básico — solo Arduino Uno; sirve para lo elemental).
- Claude escribe el código y las instrucciones; **vos lo corrés y reportás** qué pasó.

---

## 🗺️ Mapa de fases

| Fase | Nombre | Estado |
|---|---|---|
| 0 | Preparación y organización | 🔜 En curso |
| 1 | Entorno + primeros pasos | ⬜ |
| 2 | Sensores por separado | ✅ |
| 3 | Simulador de ECG (Arduino) | ✅ |
| 4 | Lógica de decisión (FSM, sin motor) | ✅ |
| 5 | Motor por separado | ✅ |
| 6 | Pantalla Nextion (HMI) | ✅ |
| 7 | Seguridad | ⬜ |
| 8 | Integración | ⬜ |
| 9 | Montaje físico + alimentación | ⬜ |
| 10 | Pruebas en el maniquí + documentación | ⬜ |

---

## FASE 0 — Preparación y organización

- [x] **T0.1** — Definir el alcance y las decisiones del proyecto 🤝
- [x] **T0.2** — Crear la carpeta del proyecto `C:\Users\Valti\Documents\core-cpr\` 🤖
- [x] **T0.3** — Crear este plan de trabajo 🤖
- [x] **T0.4** — Crear los demás documentos (COMO-TRABAJAMOS, CONVENCIONES-CODIGO, BOM, README) 🤖
- [~] **T0.5** — Definir presupuesto aproximado y de dónde se compran los componentes 🧑 · _Pospuesta: ignorada por ahora, por decisión del usuario._
- [~] **T0.6** — Confirmar qué componentes ya tienes y cuáles faltan comprar 🧑 · _Sin componentes por el momento; el usuario avisará cuando consiga alguno._

## FASE 1 — Entorno + primeros pasos

- [x] **T1.1** — Crear una cuenta gratis en Wokwi y abrir un proyecto de ESP32 🧑
- [x] **T1.2** — Primer "hola mundo": hacer parpadear un LED con la ESP32 (en Wokwi) 🤝
- [x] **T1.3** — Entender la consola serial (ver mensajes de texto) 🤝
- [x] **T1.4** — Crear el archivo `config.h` y el módulo `logger` (base de todo) 🤖
- [x] **T1.5** — (Cuando llegue) instalar el entorno para la ESP32 real 🧑 · _Lograda: Arduino IDE 1.8.19 con el paquete "ESP32 Arduino" instalado, placa "ESP32 Dev Module" seleccionada. Confirmado subiendo el sketch `Blink` por USB (COM8): el LED de la placa parpadea._

## FASE 2 — Sensores por separado (simulación primero)

- [x] **T2.1** — Módulo `sensor_ecg` + `ads1115`: leer "hay señal / no hay señal" (simulado con potenciómetro) 🤖🧑
- [x] **T2.2** — Módulo `sensor_ppg`: leer "hay pulso / no hay pulso" (simulado) 🤖🧑
- [x] **T2.3** — Módulo `sensor_fuerza` (HX711): leer fuerza en N (simulado) 🤖🧑
- [x] **T2.4** — Módulo `sensor_profundidad` (VL53L0X): leer profundidad en cm (simulado) 🤖🧑
- [x] **T2.5** — Verificar que cada sensor imprime sus valores en la consola con logs 🤝

## FASE 3 — Simulador de ECG (Arduino)

- [x] **T3.1** — Programar el Arduino para generar una señal tipo ECG 🤖🧑
- [x] **T3.2** — Agregar un botón/switch para cortar y reanudar la señal a mano 🤝
- [x] **T3.3** — (Punto F) Definir cómo se inyecta esa señal en el AD8232 y qué componentes chicos hacen falta 🤖
- [~] **T3.4** — Probar que la ESP32 detecta el corte y la reanudación 🤝 · _En stand-by: Wokwi (editor web) no permite programar dos placas distintas en un mismo proyecto. Se prueba con el Arduino y la ESP32 físicos (ver T9.4). No bloquea el avance a Fase 4._

## FASE 4 — Lógica de decisión (FSM, sin motor todavía)

- [x] **T4.1** — Diseñar la máquina de estados (dibujarla juntos) 🤖
- [x] **T4.2** — Programar `logica_fsm`: decide "vivo / paro" cruzando ECG + PPG 🤖 · _El PPG decide siempre (inicio y fin), el ECG solo se registra en logs (caso AESP). Ver `docs/diagramas/T4.1-fsm.md`._
- [x] **T4.3** — (Punto A) Al elegir "modo niño", arrancar el diagnóstico automáticamente 🤖 · _Selección de modo con switch físico (GND = niño), no con pantalla — no depende de la Fase 6. Modo embarazada descartado (el NEMA 23 no tiene fuerza para comprimir un adulto/embarazada); queda como nota a futuro. Modo adulto: no funcional, no hace ninguna lectura._
- [x] **T4.4** — (Punto C) Reevaluación cada 2 min (pausa 5 s) + parada inmediata si vuelve la vida 🤖 · _Probado en Wokwi con 15 s en vez de 2 min (`MODO_SIMULACION`); pasa a 2 min automático con hardware real. Durante COMPRIMIENDO no se lee ni se muestra ECG/PPG._
- [x] **T4.5** — Salida de prueba: encender un LED en vez del motor (para ver la decisión) 🤝 · _Probado en Wokwi: LED prendido en COMPRIMIENDO, apagado en MONITOREANDO/REEVALUANDO._

## FASE 5 — Motor por separado

- [x] **T5.1** — Módulo `actuador_motor`: mover el NEMA 23 vía STEP/DIR/ENABLE (simulado) 🤖🧑 · _Probado en Wokwi (driver A4988 + motor genérico como reemplazo del HBS57 real, mismo protocolo). Prueba de arranque: 1 cm adelante y 1 cm atrás. Pulsos/vuelta (1000) y polaridad de ENABLE (LOW) quedan como PROVISIONAL hasta el driver real (T9.4)._
- [x] **T5.2** — Secuencia de **homing** con final de carrera (posición cero) 🤖 · _Probado en Wokwi: `motorHoming()` mueve el motor hacia el extremo retraído (único punto físico fijo del riel) hasta activar el switch (simulado con pulsador), define ahí el cero. Tope de seguridad de 30 cm si el switch no se activa. Velocidad de homing (`MOTOR_HOMING_DELAY_US`) es un valor de arranque, no PROVISIONAL crítico._
- [x] **T5.3** — (Punto D) Lograr ~5 cm de profundidad y 100–120 CPM con movimiento suave 🤖 · _Probado en Wokwi: ciclo continuo bajar/subir con rampa de aceleración/desaceleración (perfil trapezoidal), a 110 CPM fijo (punto medio del rango) y reparto 50/50 bajada/subida (mismo criterio que compresores mecánicos reales, para no acortar el recoil). Movimiento no bloqueante (`motorIniciarTramo`/`motorActualizarTramo`); se quitó el `delay(500)` del `loop()` y se reemplazó por un cronómetro no bloqueante, para que el motor pueda revisarse miles de veces por segundo sin congelar el resto del programa._
- [x] **T5.4** — (Punto E) Que la ESP32 sepa la posición (conteo de pasos + homing + VL53L0X) 🤖 · _Probado en Wokwi: conteo de pasos desde el home (`motorPosicionCm()`) cruzado contra el sensor de profundidad (VL53L0X simulado). Simplificación: en esta prueba aislada el home se toma como si fuera el punto de contacto con el pecho, ya que `POSICION_LISTO_CM` (distancia real de aproximación) aún no está definida — se ajusta cuando se arme la estructura física. `TOLERANCIA_POSICION_CM` (1 cm) es un valor de arranque, ajustable._
- [~] **T5.5** — Probar límites: abortar si pasa 5 cm o 250 N 🤝 · _En stand-by: el motor todavía no está conectado a `fuerzaExcedeLimite()`/`profundidadExcedeLimite()` (solo prenden LED/log). El "abortar" real se programa en T7.2 (Seguridad); ahí se retoma esta prueba._

## FASE 6 — Pantalla Nextion (HMI)

- [x] **T6.1** — Módulo `hmi_pantalla`: muestra el modo activo 🤖 · _Redacción actualizada: ya no es un "menú" de selección — el modo se elige por switch físico (T4.3), la pantalla solo confirma cuál quedó activo ("Modo: NINO" / "Modo ADULTO - no disponible"). Probado en Wokwi: `hmiIniciar()` abre Serial2 y manda la página inicial; `hmiMostrarModo()` actualiza el texto cuando cambia el modo. Wokwi no simula la Nextion real, se verifica por los logs `[HMI]`. Pines (RX=13/TX=14) y modelo (NX3224T024) quedan PROVISIONAL hasta el hardware real (T9.4)._
- [x] **T6.2** — Adulto muestra "Modo no disponible" y no opera 🤖 · _Cubierta por T4.3 (el `return` en `loop()` corta todo en modo ADULTO) + T6.1 (`hmiMostrarModo()` ya manda el mensaje a la pantalla). No hizo falta programarla aparte. "Embarazada" descartada desde T4.3, ya no es parte del alcance._
- [x] **T6.3** — Mostrar estado del paciente y alertas en pantalla 🤖 · _Probado en Wokwi: `hmiMostrarEstado()` manda a `txtEstado` el estado clínico simple ("VIVO" / "PARO - COMPRIMIENDO" / "REEVALUANDO") cada vez que cambia el estado de la FSM; `hmiGraficarEcg()` manda el ECG (reescalado de 0–3300mV a 0–255) al componente Waveform `s0`, solo mientras no está comprimiendo. Alertas de fuerza/profundidad/paro quedaron fuera de esta tarea (a propósito, se suman en T7.2/T7.3). Componentes nuevos en la página `inicio` del editor Nextion: `txtEstado` (Text) y `s0` (Waveform, nombre autogenerado)._
- [x] **T6.4** — Botón físico "Reintentar lectura" para el caso de fallo de sensores 🤝 · _Probado en Wokwi: `botonReintentarPresionado()` (en `seguridad.h/.cpp`) detecta el click con antirebote y lo loguea (`>>> Reintentar lectura solicitado`). Pin `PIN_BOTON_REINTENTAR` = GPIO 21, PROVISIONAL (pin típico de I2C, puede reasignarse en T9.4). Por ahora solo detecta el click — la acción real de reintentar el sensor que falló se conecta en T7.2, cuando exista la lógica de fallo de sensores._

## FASE 7 — Seguridad

- [x] **T7.1** — Módulo `seguridad`: botón de paro de emergencia (corta el motor) 🤖🧑 · _Probado en Wokwi: botón real es "hongo NC" (falla de cable = mismo efecto que presionarlo, por seguridad); simulado con un switch (no un pulsador) para poder dejarlo trabado y probar el corte sin soltar. Se revisa en cada vuelta de loop() (no cada 500ms), para cortar el motor de inmediato._
- [x] **T7.2** — (Punto B) Reacción ante fallo de sensores (antes vs durante las compresiones) 🤖 · _Parte A (sesión anterior): durante la compresión, si fuerza o profundidad fallan (NAN) se aborta el motor, mismo criterio que el paro de emergencia — cierra también T5.5. Parte B: si el PPG (el sensor que decide, T4.1) falla ANTES de comprimir, la FSM no asume el paro solo — entra a un estado nuevo (`FSM_CONFIRMANDO_FALLO`), la pantalla cambia a la página `confirmar` (osciloscopio del ECG + mensaje + botones `btnSiParo`/`btnNoParo`, IDs ya confirmados en el editor Nextion) y, si nadie confirma en 10 segundos, arranca solo (fail-safe, mismo criterio AHA de "ante la duda, comprimir"). El botón "Reintentar lectura" (T6.4) ahora reactiva la confirmación si quedó silenciada tras un "No hay paro". Probado en Wokwi: la lógica de la FSM y el fail-safe por tiempo; el toque real de los botones en la pantalla queda pendiente de validar con hardware físico (T9.4)._
- [x] **T7.3** — Buzzer: alarmas acústicas (paciente vivo, fallo, límite excedido) 🤖 · _Probado en Wokwi: módulo `buzzer.h/.cpp` con tres patrones on/off (sin PWM) según prioridad — límite excedido (tono continuo) > fallo de sensor esperando confirmación (pitidos rápidos repetidos) > paciente vivo (un pitido único al confirmar). Pin `PIN_BUZZER` = GPIO 0 en simulación, PROVISIONAL: pasa a GPIO 22 en T9.4 (se libera del switch de prueba `simulador_falla`, que no existe en el circuito real). Confirmado con sonido real en el navegador para los tres casos._
- [~] **T7.4** — Finales de carrera como tope físico de seguridad 🤝 · _En stand-by: es un corte por hardware puro (cableado en serie con la alimentación del motor/ENABLE, no algo que lea el firmware), pero requiere el segundo switch y el chasis armado, que todavía no existen. Se retoma en T9.4._

## FASE 8 — Integración

- [x] **T8.1** — Unir todos los módulos en la máquina de estados completa 🤖 · _Probado en Wokwi: el motor (hasta ahora corría un ciclo de prueba independiente, T5.3) quedó conectado a la FSM — `motorCicloIniciar()` al entrar a `FSM_COMPRIMIENDO`, `motorCicloDetener()` al salir. Si el pistón iba bajando, `motorCicloDetener()` arma un tramo nuevo de subida DESDE LA POSICION ACTUAL (no desde el fondo) para no seguir empujando ni quedar trabado contra el pecho; si ya iba subiendo, deja terminar ese tramo. El paro de emergencia (T7.1) y el aborto por límite (T7.2) siguen cortando en el instante, sin cambios. Confirmado con los logs: los pasos/tiempos calculados coinciden exacto con las fórmulas. Nota aparte, no bloqueante: en Wokwi el motor se mueve mucho más lento que el CPM configurado (el simulador no da abasto con pasos cada 50-160µs) — la secuencia y los cálculos son correctos, pero el ritmo real recién se confirma con hardware en T9.4._
- [ ] **T8.2** — Probar el flujo completo en simulación de punta a punta 🤝
- [ ] **T8.3** — Ajustar la repartición de tareas entre los dos núcleos de la ESP32 🤖

## FASE 9 — Montaje físico + alimentación

- [ ] **T9.1** — Armar el chasis (perfil de aluminio, husillo, rieles, motor) 🧑
- [ ] **T9.2** — Cablear las dos fuentes aisladas (24 V motor / 5 V lógica) 🧑
- [ ] **T9.3** — Conectar sensores, driver y pantalla al circuito definitivo 🧑
- [ ] **T9.4** — **Transición a hardware**: pasar cada módulo de simulado a real (ver tabla abajo) 🤝

## FASE 10 — Pruebas en el maniquí + documentación

- [ ] **T10.1** — Prueba completa sobre el maniquí de niño 🤝
- [ ] **T10.2** — Calibrar profundidad, ritmo y fuerza contra el maniquí real 🤝
- [ ] **T10.3** — Documentar resultados y limitaciones 🤖
- [ ] **T10.4** — Preparar la presentación / defensa 🤝
- [ ] **T10.5** — (Idea futura) Evaluar crear un skill "nuevo-módulo" con el patrón ya probado 🤝

---

## 🔄 Registro de transición a hardware

> El proyecto **no está terminado** hasta que TODO tenga ✅ en la columna "Hardware".

| Subsistema | Logrado en simulación | Validado en hardware |
|---|---|---|
| ECG (AD8232 + ADS1115) | ✅ | ✅ |
| PPG (MAX30102) | ✅ | ⬜ |
| Fuerza (HX711) | ⬜ | ⬜ |
| Profundidad (VL53L0X) | ✅ | ⬜ |
| Simulador ECG (Arduino) | ⬜ | ⬜ |
| Motor (NEMA 23 + HBS57) | ⬜ | ⬜ |
| Pantalla (Nextion) | ⬜ | ⬜ |
| Seguridad (paro, buzzer, límites) | ⬜ | ⬜ |
| Lógica / FSM completa | ⬜ | ⬜ |

---

## ⏸️ Bitácora de stand-by

> Tareas que no se pudieron hacer en ningún simulador y esperan una alternativa.

| Tarea | Motivo | Plan para resolverla |
|---|---|---|
| T3.4 | Wokwi (editor web) no soporta programar dos placas distintas en un mismo proyecto | Probar con el Arduino y la ESP32 físicos cuando lleguen los componentes (ver T9.4) |
| T5.5 | El motor todavía no está conectado a los límites de fuerza/profundidad (falta la lógica de "abortar") | Se retoma en T7.2 (Seguridad), cuando se programe la reacción real ante límites excedidos |

---

## 📝 Notas técnicas pendientes (no bloquean el avance)

| Nota | Detalle | Dónde corregirlo |
|---|---|---|
| Lectura duplicada del ECG | `ecgHaySenal()` vuelve a leer el ADS1115 por su cuenta en vez de reusar un valor ya leído. Se llama por separado en 3 lugares (`firmware-esp32.ino:242,244`, `logica_fsm.cpp:31`, `logica_fsm.cpp:60`), generando hasta 3 lecturas I2C independientes por vuelta del `loop()` en vez de 1. No rompe nada hoy (el ECG no decide en la FSM), pero desperdicia tiempo I2C. | Requiere pasar el valor leído desde `firmware-esp32.ino` hasta `fsmActualizar()` (cambia `logica_fsm.h`/`.cpp` también, no solo `sensor_ecg.*`). Revisar cuando se retome la Fase 4/8 con más margen, no antes de T9.4. |

---

## ✅ Resuelto — bloqueo en prueba de hardware ECG + Pantalla

> Sesión del 2026-08-01, resuelto el 2026-08-05. `pruebas-hardware/ecg-pantalla/` — prueba aislada de AD8232+ADS1115 (ECG), VL53L0X (láser) y pantalla Nextion, como adelanto de T9.4.

- **Síntoma original:** ni el ADS1115 ni el VL53L0X se detectaban en el bus I2C ("no se encontraron dispositivos" en el Monitor Serie).
- **Causa real:** no era el código ni los pines — era la **protoboard** (contactos flojos/poco confiables). Se confirmó con un sketch nuevo (`pruebas-hardware/escaner-i2c/`) probando cada sensor **directo con jumpers, sin protoboard**: ADS1115, MAX30102 y VL53L0X aparecieron los tres, cada uno en su dirección esperada (`0x48`, `0x57`, `0x29`).
- **Validación adicional:** con otro sketch (`pruebas-hardware/lector-ecg/`) se confirmó además que el AD8232 entrega señal real a través del ADS1115 (la lectura en mV reacciona al tocar los electrodos) — el camino completo ECG (AD8232 → ADS1115 → ESP32) queda validado en hardware real.
- **Pendiente aparte (no bloquea):** la pantalla Nextion todavía no se probó en hardware real, y el sketch combinado original `ecg-pantalla/` no se volvió a correr con el cableado corregido (directo, sin protoboard) — queda para cuando se retome esa prueba puntual.
- **Lección para el resto del cableado:** evitar esa protoboard para conexiones I2C; preferir jumpers directos o soldado, al menos hasta el montaje definitivo (Fase 9).

---

## 🔧 Pendiente — probar PPG (MAX30102) y láser (VL53L0X) reales

> Sesión del 2026-08-05. Con el bloqueo de I2C ya resuelto (bloque de arriba) y el código real de `sensor_ppg.cpp`/`sensor_profundidad.cpp` ya escrito (usa las librerías SparkFun MAX3010x y Adafruit VL53L0X), faltaba probarlo en hardware. Se armaron `pruebas-hardware/lector-ppg/` y `pruebas-hardware/lector-laser/` (mismo patrón que `lector-ecg/`, con versión `.ino` y PlatformIO).

- **También en esta sesión:** se pasó `firmware-esp32/` a estructura PlatformIO (código movido a `firmware-esp32/src/`, nuevo `platformio.ini` con las 2 librerías declaradas). El flujo de Wokwi (copiar/pegar archivos en wokwi.com) sigue funcionando igual — no se vio afectado.
- **Síntoma:** `lector-ppg` no detecta el MAX30102 (`"MAX30102 no responde"`), incluso probando I2C a velocidad estándar (100kHz) en vez de rápida (400kHz) — no fue eso. El escáner sí lo había detectado antes (`0x57`), en la prueba del amigo de Lino con otro cableado.
- **Sin confirmar todavía:**
  - Si el escáner I2C (`pruebas-hardware/escaner-i2c/`) detecta `0x57` con el cableado ACTUAL de Lino, tal cual está ahora, sin moverlo.
  - Si hay algo más conectado al bus además del MAX30102 en este momento.
  - Si el cableado es el mismo que usó el amigo cuando funcionó, o se armó de nuevo.
- **Próximo paso sugerido:** correr el escáner con el cableado actual primero (sin tocar nada) para saber si es cableado flojo (mismo patrón que el ADS1115 con la protoboard) o algo más específico de la librería/chip.
- **VL53L0X (láser):** `lector-laser` todavía no se probó esta sesión (se iba a hacer después del PPG). El escáner ya lo había detectado (`0x29`) en otra prueba; falta correr la lectura real.
- **Estado:** en pausa — Lino reporta en la próxima sesión.
