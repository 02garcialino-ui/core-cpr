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
| 3 | Simulador de ECG (Arduino) | ⬜ |
| 4 | Lógica de decisión (FSM, sin motor) | ⬜ |
| 5 | Motor por separado | ⬜ |
| 6 | Pantalla Nextion (HMI) | ⬜ |
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
- [~] **T1.5** — (Cuando llegue) instalar el entorno para la ESP32 real 🧑 · _En stand-by: espera a que llegue la placa física. No bloquea el avance a Fase 2 (100% simulación)._

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
- [ ] **T4.5** — Salida de prueba: encender un LED en vez del motor (para ver la decisión) 🤝

## FASE 5 — Motor por separado

- [ ] **T5.1** — Módulo `actuador_motor`: mover el NEMA 23 vía STEP/DIR/ENABLE (simulado) 🤖🧑
- [ ] **T5.2** — Secuencia de **homing** con final de carrera (posición cero) 🤖
- [ ] **T5.3** — (Punto D) Lograr ~5 cm de profundidad y 100–120 CPM con movimiento suave 🤖
- [ ] **T5.4** — (Punto E) Que la ESP32 sepa la posición (conteo de pasos + homing + VL53L0X) 🤖
- [ ] **T5.5** — Probar límites: abortar si pasa 5 cm o 250 N 🤝

## FASE 6 — Pantalla Nextion (HMI)

- [ ] **T6.1** — Módulo `hmi_pantalla`: menú con los 3 modos 🤖
- [ ] **T6.2** — Adulto y Embarazada muestran "Modo no disponible" y no operan 🤖
- [ ] **T6.3** — Mostrar estado del paciente y alertas en pantalla 🤖
- [ ] **T6.4** — Botón físico "Reintentar lectura" para el caso de fallo de sensores 🤝

## FASE 7 — Seguridad

- [ ] **T7.1** — Módulo `seguridad`: botón de paro de emergencia (corta el motor) 🤖🧑
- [ ] **T7.2** — (Punto B) Reacción ante fallo de sensores (antes vs durante las compresiones) 🤖
- [ ] **T7.3** — Buzzer: alarmas acústicas (paciente vivo, fallo, límite excedido) 🤖
- [ ] **T7.4** — Finales de carrera como tope físico de seguridad 🤝

## FASE 8 — Integración

- [ ] **T8.1** — Unir todos los módulos en la máquina de estados completa 🤖
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
| ECG (AD8232 + ADS1115) | ⬜ | ⬜ |
| PPG (MAX30102) | ⬜ | ⬜ |
| Fuerza (HX711) | ⬜ | ⬜ |
| Profundidad (VL53L0X) | ⬜ | ⬜ |
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
