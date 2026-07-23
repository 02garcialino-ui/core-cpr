# LISTA DE MATERIALES (BOM) — Core CPR

> Prototipo académico, **solo modo niño funcional**. Motor NEMA 23, alimentación aislada de 24 V.
> 🔴 = cambió respecto al documento original · ❌ = eliminado del prototipo.

---

## A. Cerebro y control
| Cant. | Componente | Especificación | Para qué |
|---|---|---|---|
| 1 | ESP32 | DevKitC (WROOM-32) | Controlador principal |
| 1 | Buffer 74AHCT125 | — | Adapta señales 3.3V→5V limpias para el driver |
| 1 | Lote de resistencias | 220Ω, 330Ω, 1kΩ, 10kΩ, 🔴 1MΩ, 🔴 10MΩ | Pull-ups/downs, polarización y circuito de inyección de señal ECG simulada (T3.3) |

## B. Sensores
| Cant. | Componente | Especificación | Para qué |
|---|---|---|---|
| 1 | AD8232 | Con cable de electrodos | Cadena del ECG (diagnóstico) |
| 1 | ADS1115 | ADC 16-bit I2C | Digitaliza el ECG con precisión |
| 1 | MAX30102 | PPG | Pulso / oxigenación (diagnóstico) |
| 1 | Célula de carga tipo S + HX711 | 100 kg | Mide fuerza (N) → límite de seguridad ~250 N |
| 1 | Láser ToF VL53L0X | — | Mide profundidad (cm) → redundancia y límite ~5 cm |
| ❌ | ~~SCD41 (CO₂)~~ | — | **Eliminado** (lento, frágil) |

## C. Actuación y mecánica
| Cant. | Componente | Especificación | Para qué |
|---|---|---|---|
| 🔴 1 | Motor **NEMA 23 closed-loop** | ~3 Nm, **con encoder** | Genera el movimiento (antes NEMA 34) |
| 🔴 1 | Driver **HBS57** (o Rtelligent CL57) | Entrada 24–50V | Controla el motor (antes CL86T/HBS86H) |
| 1 | Husillo de bolas **SFU1610** | Largo 200–300 mm, paso 10 mm | Convierte giro en movimiento lineal (se mantiene) |
| 1 | Tuerca del husillo + soporte | Compatible SFU1610 | Une el husillo al pistón |
| 🔴 1 | **Acople** motor↔husillo | Eje 8 mm (NEMA 23) ↔ husillo | Cambia por el motor más chico |
| 🔴 1 | **Bracket/soporte** de motor | Patrón **NEMA 23** | Cambia por el motor más chico |
| 2 | Soportes de husillo | Tipo BK/BF12 (fijo + flotante) | Sostienen los extremos del husillo |
| 2 | Rieles lineales **MGN15H** | Con patines (opción MGN12H, más liviano) | Guían el pistón recto |

## D. Alimentación (aislada, dos fuentes)
| Cant. | Componente | Especificación | Para qué |
|---|---|---|---|
| 🔴 1 | Fuente 1 (potencia) | **24V, 5–6 A** | Driver + motor (antes 48V) |
| 🔴 1 | Fuente 2 (lógica) | **5V, 2–3 A**, aparte | ESP32 + sensores + pantalla (aislamiento) |
| 1 | Fusible + portafusible | Hoja **10 A** | Protege la línea de 24V |
| 1 | Botón paro emergencia | Hongo **NC** | Corte físico del motor |
| 1 | Interruptor ON/OFF | Para DC | Encendido general |
| — | Cable **AWG 18** | — | Línea de potencia |
| ❌ | ~~Fuente 48V / DC-DC aislado médico / Brake shunt / LM2596 / refrigeración~~ | — | **Eliminados** |

## E. Interfaz (HMI) y seguridad física
| Cant. | Componente | Especificación | Para qué |
|---|---|---|---|
| 1 | Pantalla Nextion | 2.8" o 3.2" | Muestra los 3 modos y alertas |
| 1 | Zumbador piezo **activo** | — | Alarmas acústicas |
| 2 | Finales de carrera | Micro switch mecánico | Homing y tope de seguridad |
| 1 | Pasador quick-release | — | Desacople físico del pistón |
| 1 | Botón físico "Reintentar" | Pulsador | Reintentar lectura tras fallo de sensores |
| ❌ | ~~Switch palanca Adulto/Pediátrico~~ | — | **Eliminado** (el modo se elige en pantalla) |

## F. Estructura y chasis
| Cant. | Componente | Especificación | Para qué |
|---|---|---|---|
| — | Perfil aluminio V-Slot | **2040** (o 2020) | Marco rígido (antes 4040) |
| — | Tuercas T M5 + tornillos Allen | — | Ensamble del chasis |
| 2 | Correas nylon | 2" ancho | Sujetan al "paciente" y el equipo |
| 2 | Hebillas Cobra | Acero/aluminio | Liberación rápida |
| 1 | Tabla espinal | Madera/acrílico/polímero | Cierra el lazo de fuerzas |
| 1 | Pistón / tacón | Silicona dura o TPU impreso | Punta que comprime el tórax |

## G. Consumibles y electrónica
| Cant. | Componente | Para qué |
|---|---|---|
| — | Jumpers + cable blindado | Conexiones (blindado para señales de sensores) |
| — | Protoboard y/o PCB | Prototipar y luego soldar |
| — | Capacitores de desacople (100nF, 10µF, 🔴 1µF) | Filtrar ruido en sensores; el 1µF es para el filtro RC de la señal ECG simulada (T3.3) |
| — | Cautín, estaño, flux, termorretráctil | Armado final |

## H. Equipo de prueba (no van dentro del dispositivo)
| Cant. | Componente | Para qué |
|---|---|---|
| 1 | **Maniquí de RCP de niño** | Sobre él se prueban las compresiones |
| 1 | **Arduino (Uno o Nano)** | Genera la señal de ECG simulada |

---

> **Nota honesta:** el torque del motor (3 Nm) y el largo del husillo son valores de arranque razonables; se confirman contra las piezas reales cuando lleguen.
