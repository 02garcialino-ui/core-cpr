# Core CPR 🫀

Dispositivo mecatrónico de **RCP (reanimación cardiopulmonar) automatizada** para un proyecto universitario.
Prototipo académico — **no es un equipo clínico certificado**.

## ¿Qué hace?

Colocado sobre un **maniquí de RCP de niño**, el dispositivo:
1. **Diagnostica** un paro cardíaco cruzando dos sensores: **ECG** (actividad eléctrica) + **PPG** (pulso).
2. Si confirma el paro, administra **compresiones torácicas mecánicas** según el estándar **AHA pediátrico**: 100–120 compresiones/min, ~5 cm de profundidad, ~250 N máximo.
3. Se **detiene** si el paciente muestra signos de vida.

## Alcance del prototipo

- ✅ **Solo el modo niño es funcional.** Los modos Adulto y Embarazada aparecen en pantalla pero no operan.
- ✅ **2 sensores de diagnóstico:** ECG (AD8232 + ADS1115) y PPG (MAX30102).
- ✅ **2 sensores de control/seguridad:** fuerza (célula de carga + HX711) y profundidad (láser VL53L0X).
- ❌ Sin sensor de CO₂ (simplificación declarada).
- ⚙️ Motor **NEMA 23** closed-loop + driver **HBS57** + husillo **SFU1610**.
- 🔌 Alimentación **aislada por dos fuentes**: 24 V (motor) y 5 V (lógica).

## Cómo se prueba (sin humano)

Un **Arduino** genera una señal de ECG simulada:
- Hay señal = "vivo" → no comprime.
- Se corta la señal = "paro" → comprime.
- Se reanuda la señal = se detiene.

El PPG se prueba con el dedo. La fuerza y la profundidad se validan sobre el maniquí real.

## Estructura del proyecto

```
Core-CPR/
├── firmware-esp32/          ← Código de la ESP32 (el cerebro)
├── simulador-ecg-arduino/   ← Código del Arduino que simula el corazón
├── docs/                    ← Documentación
│   ├── PLAN-PROYECTO.md      ← Plan de trabajo con tareas
│   ├── COMO-TRABAJAMOS.md    ← Reglas de interacción
│   ├── CONVENCIONES-CODIGO.md← Arquitectura y estilo de código
│   ├── BOM-lista-materiales.md ← Lista de materiales
│   └── diagramas/
└── README.md
```

## Estado

🚧 **En desarrollo.** Enfoque *simulación primero* (Wokwi) y luego transición a hardware.
Ver el avance en [`docs/PLAN-PROYECTO.md`](docs/PLAN-PROYECTO.md).
