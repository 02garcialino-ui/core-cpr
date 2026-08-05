#include "hmi_pantalla.h"
#include "config.h"
#include "logger.h"

// Todo comando Nextion termina con estos 3 bytes especiales, sin excepcion.
static void hmiTerminarComando()
{
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
}

// Manda un comando a la pantalla. Tambien lo deja en el log: Wokwi no
// puede dibujar la Nextion real (ver hmi_pantalla.h), asi que esta es
// la unica forma de verificar el comando mientras seguimos en simulacion.
static void hmiEnviarComando(const String &comando)
{
  Serial2.print(comando);
  hmiTerminarComando();
  logMsg(LOG_DEBUG, "HMI", "-> " + comando);
}

void hmiIniciar()
{
  Serial2.begin(NEXTION_BAUDIOS, SERIAL_8N1, PIN_NEXTION_RX, PIN_NEXTION_TX);
  delay(100); // da tiempo a que el puerto quede listo antes del primer comando
  hmiEnviarComando("page inicio");
  logMsg(LOG_INFO, "HMI", "Pantalla lista en Serial2 (RX=" + String(PIN_NEXTION_RX) + " TX=" + String(PIN_NEXTION_TX) + ")");
}

void hmiMostrarModo(ModoPaciente modo)
{
  String texto = (modo == MODO_NINO) ? "Modo: NINO" : "Modo ADULTO - no disponible";
  hmiEnviarComando("txtModo.txt=\"" + texto + "\"");
}

void hmiMostrarEstado(EstadoFsm estado)
{
  // T7.2 Parte B: el PPG fallo antes de comprimir, no se asume el paro
  // solo -- se cambia a la pagina de confirmacion en vez de actualizar
  // el texto de "inicio" (txtEstado no existe ahi, ver hmi_pantalla.h).
  if (estado == FSM_CONFIRMANDO_FALLO)
  {
    hmiEnviarComando("page confirmar");
    hmiEnviarComando("txtMensaje.txt=\"Fallo en el sensor, favor de confirmar el paro cardiaco\"");
    return;
  }

  // Para cualquier otro estado, siempre se asegura volver a "inicio" (no
  // importa si ya estabamos ahi o veniamos de "confirmar").
  hmiEnviarComando("page inicio");

  String texto;
  switch (estado)
  {
  case FSM_MONITOREANDO:
    texto = "VIVO";
    break;
  case FSM_COMPRIMIENDO:
    texto = "PARO - COMPRIMIENDO";
    break;
  case FSM_REEVALUANDO:
    texto = "REEVALUANDO";
    break;
  default:
    break; // FSM_CONFIRMANDO_FALLO ya se filtro arriba
  }
  hmiEnviarComando("txtEstado.txt=\"" + texto + "\"");
}

// El Waveform de Nextion solo acepta valores de 0 a 255. El ECG (simulado
// o real, ver sensor_ecg.cpp) llega en milivolts entre 0 y 3300, asi que
// se reescala proporcionalmente antes de mandarlo.
void hmiGraficarEcg(float valorMv)
{
  int valor = (int)((valorMv / 3300.0) * 255);
  valor = constrain(valor, 0, 255);
  hmiEnviarComando("add s0,0," + String(valor));
}

// Protocolo de toque de Nextion: 0x65 + pagina + componente + evento +
// 0xFF 0xFF 0xFF (7 bytes). evento=0x01 es "press" (se toco el boton).
// OJO: sin probar en Wokwi (ver hmi_pantalla.h) -- se valida en T9.4,
// donde tambien se confirma este formato contra el manual de la Nextion.
ToqueNextion hmiLeerToque()
{
  if (Serial2.available() < 7)
    return TOQUE_NINGUNO;

  byte b[7];
  for (int i = 0; i < 7; i++)
  {
    b[i] = Serial2.read();
  }

  bool esEventoDeToque = (b[0] == 0x65 && b[4] == 0xFF && b[5] == 0xFF && b[6] == 0xFF);
  if (!esEventoDeToque)
  {
    logMsg(LOG_WARN, "HMI", "Bytes recibidos no reconocidos (se esperaba un evento de toque)");
    return TOQUE_NINGUNO;
  }

  byte pagina = b[1];
  byte componente = b[2];
  byte evento = b[3];

  if (pagina != NEXTION_PAGINA_CONFIRMACION || evento != 0x01)
  {
    return TOQUE_NINGUNO;
  }

  if (componente == NEXTION_COMPONENTE_BTN_SI)
  {
    logMsg(LOG_WARN, "HMI", "<- Toque: boton 'SI hay paro'");
    return TOQUE_SI_HAY_PARO;
  }
  if (componente == NEXTION_COMPONENTE_BTN_NO)
  {
    logMsg(LOG_INFO, "HMI", "<- Toque: boton 'NO hay paro'");
    return TOQUE_NO_HAY_PARO;
  }

  return TOQUE_NINGUNO;
}
