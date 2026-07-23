// =====================================================
//  Core CPR - Simulador de ECG (Arduino Uno)
//  Estado: Fase 3 - T3.2 boton/switch para cortar y reanudar la senal
//
//  Este Arduino NO es parte del dispositivo de RCP. Es un aparato
//  aparte que "finge" el corazon del maniqui, generando una senal
//  parecida a un latido real para probar el resto del sistema.
// =====================================================

const int PIN_SALIDA_ECG = 9;            // pin PWM: futura entrada al AD8232 (T3.3)
const int PIN_SWITCH_CORTE = 7;          // interruptor: corta y reanuda la senal a mano
const int FRECUENCIA_CARDIACA_BPM = 75;  // latidos por minuto simulados (reposo normal)
const int VALOR_LINEA_PLANA = 128;       // "sin actividad": linea de base, sin latido

// Forma de UN latido (P-QRS-T), muestreada en puntos con valores 0-255 (PWM):
// - onda P: bultito chico (aviso de que viene un latido)
// - complejo QRS: pico agudo y alto (lo mas caracteristico del ECG)
// - onda T: onda ancha y suave
// - el resto: linea de base plana, hasta que empieza el proximo latido
const int FORMA_LATIDO[] = {
  128, 130, 132, 130, 128, 128, 128, 128,             // linea de base
  135, 140, 135, 128,                                  // onda P
  128, 120, 60, 220, 90, 128, 128,                     // complejo QRS
  128, 132, 140, 148, 152, 148, 140, 132, 128,         // onda T
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128  // linea de base
};
const int CANTIDAD_MUESTRAS = sizeof(FORMA_LATIDO) / sizeof(FORMA_LATIDO[0]);

bool habiaCorte = false;   // para avisar SOLO cuando cambia el estado del switch

void setup() {
  Serial.begin(115200);
  pinMode(PIN_SALIDA_ECG, OUTPUT);
  pinMode(PIN_SWITCH_CORTE, INPUT_PULLUP);
  Serial.println("Simulador de ECG iniciado. Abre el Serial Plotter para ver la onda.");
}

void loop() {
  // Con INPUT_PULLUP, el pin da LOW cuando el switch conecta a GND (corte activo).
  bool corteActivo = (digitalRead(PIN_SWITCH_CORTE) == LOW);

  if (corteActivo != habiaCorte) {
    if (corteActivo) {
      Serial.println("=== CORTE MANUAL: senal detenida (simula paro) ===");
    } else {
      Serial.println("=== SENAL REANUDADA (simula recuperacion) ===");
    }
    habiaCorte = corteActivo;
  }

  int valor;
  if (corteActivo) {
    valor = VALOR_LINEA_PLANA;   // linea plana: sin actividad electrica
  } else {
    unsigned long duracionLatidoMs = 60000UL / FRECUENCIA_CARDIACA_BPM;
    unsigned long tiempoEnCiclo = millis() % duracionLatidoMs;
    int indice = (tiempoEnCiclo * CANTIDAD_MUESTRAS) / duracionLatidoMs;
    valor = FORMA_LATIDO[indice];
  }

  analogWrite(PIN_SALIDA_ECG, valor);
  Serial.println(valor);

  delay(5);   // ~200 muestras por segundo: suficiente para ver bien la forma
}
