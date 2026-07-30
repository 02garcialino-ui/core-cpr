#ifndef CONFIG_H
#define CONFIG_H

// =====================================================
//  Core CPR - Configuración central
//  Todos los números del proyecto viven aquí.
//  Regla: si es un valor ajustable, va en este archivo.
// =====================================================

// ---- Interruptores globales ----
#define MODO_SIMULACION  true    // true = datos simulados | false = sensores reales
#define MODO_DEBUG       true    // true = imprime logs de nivel DEBUG

// ---- Comunicación ----
#define SERIAL_BAUDIOS   115200  // velocidad de la consola serial

// ---- Pines (ESP32 DevKitC v4) ----
// OJO: usar los pines rotulados con el número solo (2, 4, 5...).
// Los pines CLK, CMD, D0-D3 son de la memoria flash: NO se usan.
#define PIN_LED_ESTADO   2       // LED de prueba / indicador de actividad

// ---- Salida de prueba de compresion (T4.5) ----
// Reemplaza al motor, que todavia no existe (Fase 5). Prendido =
// estado COMPRIMIENDO de la FSM, apagado = MONITOREANDO/REEVALUANDO.
#define PIN_LED_COMPRESION  23

// ---- Selector de modo (T4.3) ----
// Switch fisico de 2 posiciones. GND (con INPUT_PULLUP) = modo NINO.
// Modo ADULTO: no funcional (el NEMA 23 no tiene fuerza para comprimir
// un adulto). Modo embarazada: descartado por la misma razon, queda
// solo como nota a futuro (ver PLAN-PROYECTO.md).
#define PIN_SELECTOR_MODO  19

// ---- ECG (T2.1) ----
// GPIO 34 es de SOLO ENTRADA y tiene ADC. Ideal para un potenciometro.
// En simulacion aqui va el potenciometro; con hardware real se usa el ADS1115.
#define PIN_ECG_SIM      34      // entrada simulada del ECG (potenciometro)
#define ADS1115_DIR_I2C  0x48    // direccion I2C del ADS1115 (ADDR a GND)
#define ADS1115_CANAL_ECG 0      // canal del ADS1115 donde entra el AD8232

// ---- Umbrales del ECG ----
// Por debajo de este valor consideramos que NO hay senal cardiaca.
// OJO: este numero es PROVISIONAL. Se recalibra con el sensor real (ver T9.4).
// 1500 = punto medio del rango del potenciometro (0-3300mV simulado).
#define UMBRAL_ECG_MV    1500    // milivolts

// ---- PPG (T2.2) ----
// GPIO 35 es de SOLO ENTRADA y tiene ADC, igual que el 34 del ECG.
// El sensor real (MAX30102) no esta disponible en Wokwi, asi que en
// simulacion tambien usamos un potenciometro (mismo patron que el ECG).
#define PIN_PPG_SIM      35      // entrada simulada del PPG (potenciometro)
#define PIN_LED_PPG      4       // LED que indica "hay pulso"

// ---- Umbral del PPG ----
// Por debajo de este valor consideramos que NO hay pulso.
// OJO: este numero es PROVISIONAL. Se recalibra con el sensor real (ver T9.4).
// 1650 = punto medio del rango del potenciometro (0-3300mV simulado).
#define UMBRAL_PPG_MV    1650    // milivolts

// ---- Fuerza (T2.3) ----
// El HX711 SI esta simulado en Wokwi (chip real, no un potenciometro),
// asi que el mismo codigo de lectura sirve para simulacion y para hardware real.
#define PIN_HX711_DT     16      // pin de datos (DOUT) del HX711
#define PIN_HX711_SCK    17      // pin de reloj (SCK) del HX711
#define PIN_LED_FUERZA   5       // LED que indica "fuerza excede el limite"

// Conteos crudos por kilogramo, segun el rango del chip simulado tipo "50kg"
// (0-21000 conteos = 0-50 kg). OJO: PROVISIONAL, se recalibra con el sensor
// real (ver T9.4).
#define HX711_CALIBRACION 420.0

// ---- Profundidad (T2.4) ----
// GPIO 32 tiene ADC y puede usarse como entrada, igual que el 34 y el 35.
// El sensor real (VL53L0X) no esta disponible en Wokwi, asi que en
// simulacion tambien usamos un potenciometro (mismo patron que ECG/PPG).
#define PIN_PROFUNDIDAD_SIM   32   // entrada simulada de profundidad (potenciometro)
#define PIN_LED_PROFUNDIDAD   18  // LED que indica "profundidad excede el limite"

// Rango simulado del potenciometro: 0-10 cm (no el rango real del VL53L0X,
// que es mucho mayor). Se elige asi para tener buena resolucion alrededor
// del limite de 5 cm. OJO: PROVISIONAL, se recalibra con el sensor real (T9.4).
#define PROFUNDIDAD_SIM_MAX_CM 10.0

// ---- Motor (T5.1) ----
// Protocolo STEP/DIR/ENABLE: funciona igual con el HBS57 real que con el
// A4988 usado en Wokwi como reemplazo visual (mismo protocolo). Cambiar de
// driver es solo cableado; el codigo no cambia.
#define PIN_MOTOR_STEP     25
#define PIN_MOTOR_DIR      26
#define PIN_MOTOR_ENABLE   27

// La mayoria de los drivers STEP/DIR (A4988, HBS57 incluido) habilitan el
// motor con LOW. OJO: PROVISIONAL, confirmar con el datasheet del HBS57
// real cuando llegue (ver T9.4).
#define MOTOR_ENABLE_ACTIVO_BAJO  true

// Pulsos por vuelta configurados en el driver (microstepping). El NEMA23
// da 200 pasos/vuelta de fabrica; el HBS57 los subdivide segun sus
// dip-switches. OJO: PROVISIONAL, se ajusta con el driver real (T9.4).
#define MOTOR_PULSOS_POR_VUELTA  1000

// El husillo SFU1610 (ver BOM) tiene paso de 10mm = 1cm por vuelta. Este
// SI es un dato real y confirmado, no PROVISIONAL.
#define HUSILLO_CM_POR_VUELTA  1.0

// Pasos necesarios para mover 1 cm (derivado de los dos valores de arriba).
#define MOTOR_PASOS_POR_CM  (MOTOR_PULSOS_POR_VUELTA / HUSILLO_CM_POR_VUELTA)

// Ancho del pulso STEP (tiempo en alto antes de bajarlo). 5us es un valor
// tipico y seguro para drivers step/dir; se ajusta si el HBS57 real pide
// un minimo distinto (ver datasheet).
#define MOTOR_PULSO_US  5

// ---- Homing (T5.2) ----
// Final de carrera del extremo RETRAIDO (lejos del paciente). Es el unico
// punto fisico fijo del riel: la profundidad de compresion depende del
// paciente (su torax, que varia), pero el extremo retraido es siempre el
// mismo tope mecanico del chasis. Por eso el homing se hace ahi.
#define PIN_FINAL_CARRERA_HOME  33

// Velocidad mas lenta durante el homing, para no golpear fuerte el tope
// mecanico al encontrarlo.
#define MOTOR_HOMING_DELAY_US  1500

// Tope de seguridad: si el homing recorre mas que el largo maximo del
// husillo (30 cm, ver BOM SFU1610: 200-300mm) sin encontrar el switch,
// algo esta mal (switch desconectado o roto) y hay que abortar en vez de
// girar el motor indefinidamente.
#define HOMING_MAX_PASOS  ((long)(MOTOR_PASOS_POR_CM * 30))

// ---- Ritmo y profundidad de compresion (T5.3 / Punto D) ----
// CPM objetivo fijo dentro del rango permitido (100-120). 110 = punto medio.
#define RITMO_CPM_OBJETIVO  110

// Duracion de un ciclo completo (bajar + subir), segun el ritmo objetivo.
#define DURACION_CICLO_MS   (60000UL / RITMO_CPM_OBJETIVO)

// Reparto del ciclo: mitad bajando, mitad subiendo. Mismo criterio que usan
// los compresores mecanicos reales (ej. LUCAS): igualar tiempo de compresion
// y de relajacion para no acortar el "recoil" del torax (perfusion coronaria).
#define DURACION_BAJADA_MS  (DURACION_CICLO_MS / 2)
#define DURACION_SUBIDA_MS  (DURACION_CICLO_MS / 2)

// Pasos totales de una compresion completa (ida), segun PROFUNDIDAD_MAX_CM
// (limite clinico AHA ya definido, no es un valor nuevo).
#define PASOS_COMPRESION  ((long)(MOTOR_PASOS_POR_CM * PROFUNDIDAD_MAX_CM))

// Fraccion del recorrido dedicada a acelerar/desacelerar (rampa); el resto
// va a velocidad de crucero constante. 20% es un valor de arranque tipico
// para un perfil trapezoidal simple.
#define FRACCION_RAMPA  0.2

// ---- Posicion y cruce con VL53L0X (T5.4 / Punto E) ----
// Cuanto puede diferir la posicion calculada por conteo de pasos y la
// medida por el sensor de profundidad antes de avisar. Un valor chico
// es sensible a ruido de mas (falsas alarmas); uno grande no detecta
// perdida real de pasos. 1 cm es un punto de partida razonable.
#define TOLERANCIA_POSICION_CM  1.0

// ---- Tiempos de la FSM (T4.4) ----
// En MODO_SIMULACION se usa un tiempo de compresion corto (15 s) para
// poder probar en Wokwi sin esperar los 2 minutos reales. Con hardware
// real (MODO_SIMULACION false) se usan los 2 minutos del estandar AHA.
#if MODO_SIMULACION
  #define DURACION_COMPRESION_MS   (15UL * 1000UL)
#else
  #define DURACION_COMPRESION_MS   (2UL * 60UL * 1000UL)
#endif
#define DURACION_REEVALUACION_MS  (5UL * 1000UL)   // pausa para revisar (AHA limita a ~10s)

// ---- Confirmacion de fallo del PPG (T7.2 Parte B) ----
// Si el PPG (el sensor que decide el paro, T4.1) falla ANTES de comprimir,
// no se asume el paro solo: se pide confirmar a mano en la pantalla. Si
// nadie confirma en este tiempo, arranca solo (fail-safe hacia la accion,
// mismo criterio AHA que "ante la duda, comprimir").
#define DURACION_CONFIRMACION_MS  (10UL * 1000UL)

// Pagina y componentes de la pantalla de confirmacion (editor Nextion).
// Ya confirmados contra la pagina real "confirmar" (con el texto
// "txtMensaje" y los botones "btnSiParo"/"btnNoParo", con "Send
// Component ID" activado en su Touch Press Event). Sin probar en Wokwi
// (no simula la pantalla real, T6.1); se valida con hardware real en
// T9.4, incluido el evento de toque que manda la pantalla.
#define NEXTION_PAGINA_CONFIRMACION   1
#define NEXTION_COMPONENTE_BTN_SI     2
#define NEXTION_COMPONENTE_BTN_NO     3

// ---- Seguridad: paro de emergencia (T7.1) ----
// Boton tipo "hongo NC" (normalmente cerrado, ver BOM): el circuito esta
// CERRADO (LOW) mientras no se presiona. Al presionarlo, O si el cable se
// corta/desconecta, el circuito se ABRE (HIGH) -- misma reaccion en ambos
// casos, a proposito (una falla se trata igual que una emergencia real).
// En Wokwi se simula con un switch (no un pulsador), para poder dejarlo
// "trabado" en la posicion de emergencia y probar el efecto sin soltarlo.
#define PIN_PARO_EMERGENCIA  15

// ---- Seguridad: boton "Reintentar lectura" (T6.4) ----
// Pulsador normal (no NC como el de emergencia). GPIO 21: es el unico pin
// libre con soporte de pull-up que queda disponible. OJO, PROVISIONAL: es
// uno de los pines tipicos de I2C (SDA) -- puede que haya que reasignarlo
// cuando el ADS1115/VL53L0X reales se conecten por I2C de verdad (T9.4).
#define PIN_BOTON_REINTENTAR  21
#define DEBOUNCE_BOTON_MS     50    // tiempo de antirebote (ms)

// ---- SOLO PARA PRUEBAS: simular falla de sensores (T7.2) ----
// Ver simulador_falla.h. Ultimo pin libre con pull-up disponible.
#define PIN_SIM_FALLA_SENSORES  22

// ---- Pantalla Nextion (T6.1) ----
// Puerto serial aparte (Serial2), separado de los pines ya usados por el
// HX711 y el motor. La pantalla NO elige el modo (eso lo hace el switch
// fisico, T4.3): solo confirma cual quedo activo.
// Modelo NX3224T024 (2.4", serie Basica): OJO, PROVISIONAL -- se confirma
// contra la caja real cuando llegue (ver T9.4).
// Wokwi no simula esta pantalla: los comandos solo se verifican por los
// logs (ver hmi_pantalla.cpp), la prueba visual real queda para T9.4.
#define PIN_NEXTION_RX   13     // ESP32 recibe (se conecta al TX de la Nextion)
#define PIN_NEXTION_TX   14     // ESP32 transmite (se conecta al RX de la Nextion)
#define NEXTION_BAUDIOS  9600   // velocidad de fabrica de la Nextion

// ---- Límites clínicos (AHA - modo niño) ----
// Fuente: estándar AHA pediátrico, según se definió en T0.1.
#define PROFUNDIDAD_MAX_CM   5.0   // profundidad máxima de compresión (cm)
#define FUERZA_MAX_N         250   // fuerza máxima admitida (N)
#define RITMO_CPM_MIN        100   // ritmo mínimo (compresiones por minuto)
#define RITMO_CPM_MAX        120   // ritmo máximo (compresiones por minuto)

#endif
