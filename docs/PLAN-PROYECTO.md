# PLAN DE PROYECTO — Core CPR

> Dispositivo de RCP automatizada (prototipo académico, **solo modo niño funcional**).
> Documento vivo: lo vamos marcando a medida que avanzamos.

---

## 🎯 Objetivo del proyecto

Construir un dispositivo que, colocado sobre un **maniquí de RCP de niño**, detecte un paro cardíaco (mediante ECG + PPG) y administre **compresiones torácicas mecánicas** según el estándar AHA pediátrico: **100–120 compresiones por minuto, ~5 cm de profundidad, ~250 N máximo de fuerza**.

**Cómo se prueba:** un **Arduino** genera una señal de ECG simulada. Con señal = "vivo" (no comprime). Se corta la señal = "paro" (comprime). Se reanuda la señal = se detiene.

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
| 2 | Sensores por separado | ⬜ |
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

- [ ] **T1.1** — Crear una cuenta gratis en Wokwi y abrir un proyecto de ESP32 🧑
- [ ] **T1.2** — Primer "hola mundo": hacer parpadear un LED con la ESP32 (en Wokwi) 🤝
- [ ] **T1.3** — Entender la consola serial (ver mensajes de texto) 🤝
- [ ] **T1.4** — Crear el archivo `config.h` y el módulo `logger` (base de todo) 🤖
- [ ] **T1.5** — (Cuando llegue) instalar el entorno para la ESP32 real 🧑

## FASE 2 — Sensores por separado (simulación primero)

- [ ] **T2.1** — Módulo `sensor_ecg` + `ads1115`: leer "hay señal / no hay señal" (simulado con potenciómetro) 🤖🧑
- [ ] **T2.2** — Módulo `sensor_ppg`: leer "hay pulso / no hay pulso" (simulado) 🤖🧑
- [ ] **T2.3** — Módulo `sensor_fuerza` (HX711): leer fuerza en N (simulado) 🤖🧑
- [ ] **T2.4** — Módulo `sensor_profundidad` (VL53L0X): leer profundidad en cm (simulado) 🤖🧑
- [ ] **T2.5** — Verificar que cada sensor imprime sus valores en la consola con logs 🤝

## FASE 3 — Simulador de ECG (Arduino)

- [ ] **T3.1** — Programar el Arduino para generar una señal tipo ECG 🤖🧑
- [ ] **T3.2** — Agregar un botón/switch para cortar y reanudar la señal a mano 🤝
- [ ] **T3.3** — (Punto F) Definir cómo se inyecta esa señal en el AD8232 y qué componentes chicos hacen falta 🤖
- [ ] **T3.4** — Probar que la ESP32 detecta el corte y la reanudación 🤝

## FASE 4 — Lógica de decisión (FSM, sin motor todavía)

- [ ] **T4.1** — Diseñar la máquina de estados (dibujarla juntos) 🤖
- [ ] **T4.2** — Programar `logica_fsm`: decide "vivo / paro" cruzando ECG + PPG 🤖
- [ ] **T4.3** — (Punto A) Al elegir "modo niño", arrancar el diagnóstico automáticamente 🤖
- [ ] **T4.4** — (Punto C) Reevaluación cada 2 min (pausa 5 s) + parada inmediata si vuelve la vida 🤖
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

*(Vacía por ahora.)*

| Tarea | Motivo | Plan para resolverla |
|---|---|---|
| — | — | — |
