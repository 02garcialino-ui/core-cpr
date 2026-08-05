# Diseño mecánico — Plano arquitectónico (Modo niño)

> Estructura mecánica del dispositivo: arco de compresión torácica tipo **LUCAS** (Stryker), a escala pediátrica. Documento vivo — se ajusta en **T9.1** cuando se arme el chasis real.

## Nota honesta

Todavía no hay un maniquí físico para medir, y los planos reales del LUCAS son propiedad de Stryker (no públicos). Las medidas marcadas **PROVISIONAL** son estimaciones de antropometría pediátrica típica (niño ≈ 6 años, ni bebé ni adulto) o supuestos mecánicos razonables — no son medidas confirmadas. Se corrigen contra la pieza real en T9.1.

## Idea general

Igual que el LUCAS real: una **tabla espinal** va bajo la espalda del niño, y desde sus bordes suben dos **columnas** que se unen arriba en una **viga**, formando un arco sobre el pecho. Del centro de esa viga cuelga el mecanismo que empuja: motor → husillo de bolas → pistón, todo guiado por un riel lineal en paralelo (el husillo empuja, el riel aguanta la carga lateral). Correas sujetan el conjunto al tórax, como en el original.

La tabla espinal (A) es **desmontable** de las columnas (B) — no una sola pieza fija. Ver el detalle en "Unión tabla espinal ↔ columnas" más abajo.

## Vistas

### Vista lateral (perfil, corte a la altura del esternón)

```
                       ┌─────────┐
                       │    D    │   Motor NEMA23 (closed-loop)
                       └────┬────┘
          ┌──────────────────────────────────┐
          │             C — viga              │
          └──┬────────────────────────────┬───┘
             │ E — acople flexible          │
             │ F│G — husillo (SFU1610)       ┊  K — VL53L0X (fijo, apunta
             │      + riel (MGN15H)          ┊       al pecho junto al pistón)
             │                               ┊
   ┄┄┄┄┄┄┄┄┄┄┤ L — home (switch superior)     ┊     21 cm  (home)
             │
          ┌──┴──┐
          │  H  │  carro (patín + tuerca)          16 cm  (listo)
          └──┬──┘
          ┌──┴──┐
          │  I  │  celda de carga (HX711)
          └──┬──┘
            (J)   pistón/tacón           ┄┄┄┄┄┄     13 cm  (pecho en reposo)
   ┄┄┄┄┄┄┄┄┄┄┴┄┄┄┄  L — tope inferior (T7.4)          8 cm  (comprimido, -5cm)
 ══════════════════════════════════════════════
              A — tabla espinal (45 × 36 × 1 cm)

   (B — columna: a los costados del tórax, fuera de este corte —
        ver vista frontal)
```

### Vista frontal (transversal, a la altura del pecho)

```
      B                                    B
   ┌────┐                              ┌────┐
   │    │        ┌────D────┐           │    │
   │    ├────────┤  viga C  ├──────────┤    │
   │    │   E     └────┬────┘           │    │
   │    │           F–J (husillo, riel, │    │   K — VL53L0X
   │    │           carro, celda,       │    │       (junto al eje central)
   │    │           pistón — ver        │    │
   │    │           vista lateral)      │    │
   │    │                │              │    │
   │    │             ( pistón )        │    │
   │    │  ┄┄┄┄┄┄┄┄┄┄┄┄┄┄┴┄┄┄┄┄┄┄┄┄┄┄┄┄  │    │   M — correas + hebillas
   └────┴──────────────█████──────────────┴────┘        (rodean el tórax)
   |←───────────────  36 cm  ───────────────→|
        |←────────── 28 cm interior ───────→|
```

> Diagrama acotado y a escala (SVG interactivo, con las mismas referencias A–N): ver el plano publicado — [Core CPR — Plano arquitectónico mecánico](https://claude.ai/code/artifact/1de930bf-00f7-43ba-987b-bbf2453134cf) (enlace privado del usuario, no público).

## Lista de componentes

| Ref | Componente | Ubicación | Medida / especificación |
|---|---|---|---|
| A | Tabla espinal (backboard) | Base, bajo la espalda del niño | 45 × 36 × 1 cm **PROV** — acrílico o madera |
| B | Columnas laterales (×2) | Bordes de la tabla, a los costados del tórax | perfil alum. 2020, 40 cm alto **PROV** |
| C | Viga superior (crossbeam) | Une las dos columnas, arriba del pecho | perfil alum. 2020, 32 cm |
| D | Motor NEMA 23 closed-loop | Arriba de la viga, eje hacia abajo | ~5.7 × 9 cm **PROV** |
| E | Acople flexible eje 8 mm | Bajo el motor, entra a la columna | une motor ↔ husillo |
| F | Husillo de bolas SFU1610 | Vertical, centrado en la columna | 250 mm, paso 10 mm/vuelta |
| G | Riel lineal MGN15H | Paralelo al husillo | ~250 mm — absorbe carga lateral |
| H | Carro (patín + tuerca) | Parte móvil sobre husillo + riel | recorrido útil ≈ 15–18 cm |
| I | Celda de carga tipo S + HX711 | En línea, entre el carro y el pistón | mide fuerza real (límite 250 N) |
| J | Pistón / tacón | Punta, contacto con el pecho | silicona/TPU, ⌀ ≈ 4 cm **PROV** |
| K | Láser VL53L0X | Fijo bajo la viga, junto al pistón | apunta al pecho, sin obstruir a J |
| L | Finales de carrera (×2) | Sobre una columna | uno arriba (home), uno abajo (tope T7.4) |
| M | Correas + hebillas Cobra | Bordes de la tabla, rodean el tórax | 2" ancho, liberación rápida |
| N | Caja de electrónica | Aparte, al costado — no sobre el arco | ESP32, Nextion, HBS57, fuentes, buzzer, paro de emergencia |
| — | AD8232 (ECG) + MAX30102 (PPG) | Cables sueltos hacia el paciente | no forman parte del arco móvil |

## Por qué estas medidas

**Altura de columna — 40 cm.** Se arma sumando, desde la tabla hacia arriba: profundidad de tórax en reposo, holgura de aproximación, carrera de compresión, y una reserva mecánica arriba para el acople, el soporte del husillo y el margen de homing.

```
13 (pecho en reposo) + 3 (holgura "listo") + 5 (carrera AHA pediátrica) + 19 (reserva mecánica) = 40 cm
```

**Ancho de tabla — 36 cm.** Ancho de hombros estimado (24 cm) más holgura para que las columnas no aprieten los brazos, más margen de borde para anclar las correas.

```
24 (hombros, estimado) + 2×2 (holgura) + 2×2 (borde) = 32 → 36 cm
```

**Husillo SFU1610 — 250 mm.** Del rango del BOM (200–300 mm) se elige el punto medio: descontando los soportes BK/BF12 en los extremos, deja ≈15–18 cm de recorrido útil — cubre los 13 cm que necesita el ciclo (home → listo → comprimido) con margen para el homing.

**Riel MGN15H en paralelo.** El husillo de bolas está pensado para empujar, no para cargar peso lateral. El riel lineal absorbe esa carga y evita que el husillo se flexione o se trabe.

## Por qué la caja de electrónica va separada del arco

El arco (columnas + viga + mecanismo) es la parte que se mueve y que carga sobre el pecho del niño. Meter ahí la ESP32, la pantalla, el driver y las fuentes agregaría peso y vibración justo donde no conviene. Por eso quedan en una caja aparte, conectada por cable — mismo criterio de separación de masas que usa el LUCAS real (unidad de control separada del brazo de compresión).

## Próximo paso

**T9.1** — armar el chasis y confirmar contra las piezas reales todas las medidas marcadas **PROV** en este documento (sobre todo las que dependen del maniquí: A, B, D, J).
