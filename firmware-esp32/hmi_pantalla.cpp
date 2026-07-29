#include "hmi_pantalla.h"
#include "config.h"
#include "logger.h"

// Todo comando Nextion termina con estos 3 bytes especiales, sin excepcion.
static void hmiTerminarComando() {
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
}

// Manda un comando a la pantalla. Tambien lo deja en el log: Wokwi no
// puede dibujar la Nextion real (ver hmi_pantalla.h), asi que esta es
// la unica forma de verificar el comando mientras seguimos en simulacion.
static void hmiEnviarComando(const String& comando) {
  Serial2.print(comando);
  hmiTerminarComando();
  logMsg(LOG_DEBUG, "HMI", "-> " + comando);
}

void hmiIniciar() {
  Serial2.begin(NEXTION_BAUDIOS, SERIAL_8N1, PIN_NEXTION_RX, PIN_NEXTION_TX);
  delay(100);   // da tiempo a que el puerto quede listo antes del primer comando
  hmiEnviarComando("page inicio");
  logMsg(LOG_INFO, "HMI", "Pantalla lista en Serial2 (RX=" + String(PIN_NEXTION_RX) +
                          " TX=" + String(PIN_NEXTION_TX) + ")");
}

void hmiMostrarModo(ModoPaciente modo) {
  String texto = (modo == MODO_NINO) ? "Modo: NINO" : "Modo ADULTO - no disponible";
  hmiEnviarComando("txtModo.txt=\"" + texto + "\"");
}

void hmiMostrarEstado(EstadoFsm estado) {
  String texto;
  switch (estado) {
    case FSM_MONITOREANDO: texto = "VIVO"; break;
    case FSM_COMPRIMIENDO: texto = "PARO - COMPRIMIENDO"; break;
    case FSM_REEVALUANDO:  texto = "REEVALUANDO"; break;
  }
  hmiEnviarComando("txtEstado.txt=\"" + texto + "\"");
}

// El Waveform de Nextion solo acepta valores de 0 a 255. El ECG (simulado
// o real, ver sensor_ecg.cpp) llega en milivolts entre 0 y 3300, asi que
// se reescala proporcionalmente antes de mandarlo.
void hmiGraficarEcg(float valorMv) {
  int valor = (int)((valorMv / 3300.0) * 255);
  valor = constrain(valor, 0, 255);
  hmiEnviarComando("add s0,0," + String(valor));
}
