#include "hmi_pantalla.h"
#include "config.h"
#include "logger.h"

// Version recortada de firmware-esp32/hmi_pantalla.cpp (ver hmi_pantalla.h
// de esta carpeta). El protocolo Nextion es identico al original: texto
// plano por Serial2, terminado en 3 bytes 0xFF.

static void hmiTerminarComando() {
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
}

static void hmiEnviarComando(const String &comando) {
  Serial2.print(comando);
  hmiTerminarComando();
  logMsg(LOG_DEBUG, "HMI", "-> " + comando);
}

void hmiIniciar() {
  Serial2.begin(NEXTION_BAUDIOS, SERIAL_8N1, PIN_NEXTION_RX, PIN_NEXTION_TX);
  delay(100);
  hmiEnviarComando("page inicio");
  logMsg(LOG_INFO, "HMI", "Pantalla lista en Serial2 (RX=" + String(PIN_NEXTION_RX) + " TX=" + String(PIN_NEXTION_TX) + ")");
}

void hmiGraficarEcg(float valorMv) {
  int valor = (int)((valorMv / 3300.0) * 255);
  valor = constrain(valor, 0, 255);
  hmiEnviarComando("add s0,0," + String(valor));
}

void hmiMostrarTexto(const String &texto) {
  hmiEnviarComando("txtEstado.txt=\"" + texto + "\"");
}
