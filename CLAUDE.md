# Core CPR — Instrucciones para Claude

Dispositivo mecatrónico de RCP automatizada. Prototipo académico universitario.
El usuario (**Lino**) es **novato** en electrónica, programación y uso de IA.

## ⚠️ Al inicio de CADA sesión (obligatorio, antes de responder cualquier cosa)

1. Lee `docs/COMO-TRABAJAMOS.md` — las 11 reglas de interacción acordadas.
2. Lee `docs/PLAN-PROYECTO.md` — el estado real del proyecto.
3. Dile a Lino, de forma breve:
   - En qué **fase** estamos.
   - Qué se **cerró** desde antes.
   - Cuál es la **próxima tarea pendiente** (con su código, ej. `T1.1`) y **quién la hace** (🤖 Claude / 🧑 Lino / 🤝 ambos).
4. Pregunta si arrancamos con esa tarea.

**Sin saludos largos ni motivación.** Directo al grano: dónde estamos y qué sigue.

## Reglas que nunca se rompen

- **Confirmación obligatoria:** nunca crear ni editar un archivo sin un "sí" explícito de Lino.
- **Español de Latinoamérica**, usando "tú".
- **Explicar el porqué**, no solo el qué — es un proyecto de escuela y hay que poder defenderlo.
- **Nunca inventar datos.** Si no estás seguro, dilo.
- **Separar tareas:** 🤖 lo que hace Claude / 🧑 lo que hace Lino (comprar, cablear, medir, probar).
- **De a poco:** una tarea a la vez; no se pasa de fase sin cerrar la anterior.

(Las 11 reglas completas están en `docs/COMO-TRABAJAMOS.md`. Léelas, no te fíes solo de este resumen.)

## Mantener el plan al día

`docs/PLAN-PROYECTO.md` es la única fuente de verdad sobre el avance.
Cuando una tarea se logre, **pide permiso** y márcala:

- `[ ]` pendiente · `[x]` lograda · `[-]` no se pudo en este simulador · `[~]` en stand-by

## Documentos del proyecto

| Archivo | Para qué |
|---|---|
| `docs/PLAN-PROYECTO.md` | Fases, tareas y estado (dónde quedamos) |
| `docs/COMO-TRABAJAMOS.md` | Las 11 reglas de interacción |
| `docs/CONVENCIONES-CODIGO.md` | Arquitectura y estilo del firmware |
| `docs/DISENO-MECANICO.md` | Plano arquitectónico de la estructura mecánica (arco tipo LUCAS) |
| `docs/DISENO-ELECTRICO.md` | Plano de conexiones, alimentación y pines de cada sensor/actuador |
| `docs/BOM-lista-materiales.md` | Lista de materiales |
| `README.md` | Resumen del proyecto |
