#include "buzzer.h"
#include "config.h"
#include "logger.h"

static PatronBuzzer g_patronActual = BUZZER_NINGUNO;
static unsigned long g_ultimoCambioMs = 0;
static bool g_pinEncendido = false;
static bool g_vivoYaSono = false;   // BUZZER_VIVO ya emitio su pitido unico

void buzzerIniciar() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  logMsg(LOG_INFO, "BUZZER", "Buzzer listo en GPIO " + String(PIN_BUZZER));
}

void buzzerFijarPatron(PatronBuzzer patron) {
  if (patron == g_patronActual) {
    return;   // sin cambios: no reinicia el pitido unico de BUZZER_VIVO
  }
  g_patronActual = patron;
  g_ultimoCambioMs = millis();
  g_pinEncendido = false;
  g_vivoYaSono = false;
  logMsg(LOG_DEBUG, "BUZZER", "Patron cambiado a " + String((int)patron));
}

void buzzerActualizar() {
  unsigned long ahora = millis();

  switch (g_patronActual) {
    case BUZZER_NINGUNO:
      digitalWrite(PIN_BUZZER, LOW);
      break;

    case BUZZER_VIVO:
      if (g_vivoYaSono) {
        digitalWrite(PIN_BUZZER, LOW);
      } else if (ahora - g_ultimoCambioMs < BUZZER_VIVO_DURACION_MS) {
        digitalWrite(PIN_BUZZER, HIGH);
      } else {
        digitalWrite(PIN_BUZZER, LOW);
        g_vivoYaSono = true;
      }
      break;

    case BUZZER_FALLO:
      if (ahora - g_ultimoCambioMs >= BUZZER_FALLO_INTERVALO_MS) {
        g_ultimoCambioMs = ahora;
        g_pinEncendido = !g_pinEncendido;
        digitalWrite(PIN_BUZZER, g_pinEncendido ? HIGH : LOW);
      }
      break;

    case BUZZER_LIMITE:
      digitalWrite(PIN_BUZZER, HIGH);
      break;
  }
}
