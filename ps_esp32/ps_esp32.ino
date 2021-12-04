/***************************************************************************
   Copyright (C) 2021 by Pablo Gonzalez <pgonzal@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

/*
   Transmisor wifi de lecturas analógicas en modo deepsleep
*/

#include <WiFi.h>
#include <EEPROM.h>
#include "driver/adc.h"
#include "common.h"
#include "ps_i2c.h"
#include "display.h"

RTC_DATA_ATTR unsigned long rtc_last_sample_time; // El tiempo de muestra actual está guardada en la memoria del RTC en el ciclo previo
volatile uint8_t flag_sleep_mode = 1;  // 0: no sleep o modem sleep dependiendo de Ts    1: deep sleep
volatile uint32_t t_last_touch = -1;

WiFiClient client;
IPAddress server_ip;

void setModemSleep() {
  adc_power_off();
  WiFi.disconnect();      // Disconnect from the network
  WiFi.mode(WIFI_OFF);    // Switch WiFi off
  logMsg(log_, "WiFi disconnected!");

  // Quite unusefully, no relevable power consumption
  //btStop();
  //logMsg(log_, "Bluetooth stop!");

  if (!setCpuFrequencyMhz(40)) {
    logMsg(log_, "Not valid frequency!");
  }
  // Use this if 40Mhz is not supported
  // setCpuFrequencyMhz(80);
}

void wakeModemSleep() {
  setCpuFrequencyMhz(240);
  adc_power_on();
  delay(200);
  WiFi.disconnect(false);  // Reconnect the network
  WiFi.mode(WIFI_STA);    // Switch WiFi off
  delay(200);
}

void cycleSleep(unsigned long t_sample)
{
  long t_sleep;

  if (flag_sleep_mode == 1) // Deep sleep
  {
    if ( t_sample == 0 )
      t_sleep = SAMPLE_TIME_DEEP_SLEEP / 2;
    else
      t_sleep = SAMPLE_TIME_DEEP_SLEEP - (millis() - t_sample);
    if ( t_sleep < 0 )
      t_sleep = SAMPLE_TIME_DEEP_SLEEP;
    logData(log_, "Durmiendo PROFUNDAMENTE [ms]", t_sleep);
    rtc_last_sample_time += millis() + t_sleep;

    // Antes de ir a deep sleep tengo que apagar el WiFi y el display (por si estaba prendido)
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    displaySetPowerSave(true);

    // Configura Touchpad para despertar. Puede ser útil cuando quiero escanear continuo y no quiero esperar el próximo reset
    esp_sleep_enable_touchpad_wakeup();
    esp_sleep_enable_timer_wakeup(t_sleep * 1000);
    esp_deep_sleep_start();
  }
  else // Modem sleep
  {
    if ( t_sample == 0 )
      t_sleep = SAMPLE_TIME;
    else
      t_sleep = SAMPLE_TIME - (millis() - t_sample);
    if ( t_sleep < 0 )
      t_sleep = SAMPLE_TIME;
    logData(log_, "Durmiendo [ms]", t_sleep);
    rtc_last_sample_time += millis() + t_sleep;

    // Solo paso a modem-sleep si vale la pena el tiempo en que apago la antena
    if ( t_sleep > 10000 )
    {
      setModemSleep();
      delay(1);
      delay(t_sleep - 2);
      wakeModemSleep();
      delay(1);
    }
    else
      delay(t_sleep);
  }
}

int waitForConnection()
{
  int wifi_status;
  wifi_status = WiFi.status();
  if ( wifi_status == WL_CONNECTED )
    return 0;

  logMsg(log_, "Esperando conectar a WiFi");

  int retry_counter_wifi = 0;
  while (wifi_status != WL_CONNECTED)
  {
    delay(500);
    retry_counter_wifi++;
    if ( retry_counter_wifi >= NB_RETRY_WIFI )
    {
      logMsg(log_, "(!) -----> Imposible conectar a red WiFi");
      logMsg(log_, "****************************************");
      if (log_)
        WiFi.printDiag(Serial);
      logMsg(log_, "****************************************");
      return -1;
    }
    wifi_status = WiFi.status();
  }

  logData(log_, "Conectado a WiFi con IP", WiFi.localIP());
  return 0;
}

int sockConnect()
{
  int retry_counter_socket = 0;

  logData(log_, "Intento conectar con host", server_ip);
  logData(log_, "puerto", port);

  while (!client.connect(server_ip, port))
  {
    retry_counter_socket++;
    if ( retry_counter_socket >= NB_RETRY_SOCKET )
    {
      logMsg(log_, "(!) -----> Imposible conectar con Host");
      return -1;
    }
    delay(500);
  }
  return 0;
}

void sockSendMeasurement(uint16_t sensor_status, float pres_clin, float temp, unsigned long t_sample)
{
  char buffer[100];
  sprintf(buffer, "%7u;%7.2f;%7.2f;%10lu", sensor_status, pres_clin, temp, (millis() - t_sample));
  logData(log_, "Mensaje al host", buffer);
  client.print(buffer);
}

void startEEPROM()
{
  EEPROM.begin(sizeof(TPersistentData));
}

void writeDataToEEPROM(TPersistentData &data)
{
  EEPROM.put(0, data);
  EEPROM.commit();
}

int readDataFromEEPROM(TPersistentData &data)
{
  EEPROM.get(0, data);
  return data.flag_paired;
}

void connectAP(String ssid_str)
{
  WiFi.mode(WIFI_STA);
  WiFi.hostname("ESP32-ps");

  logData(log_, "Intentando conectar a AP", ssid_str);
  logData(log_, "Password utilizada", password);

  char ssid[255];
  ssid_str.toCharArray(ssid, ssid_str.length() + 1);
  WiFi.begin((char*)ssid, (char*)password);
}

int selectAP()
{
  int i = 0, n = 0, n_iap = 0;
  int idx_iap[255];

  displayMsg("Setear WiFi");
  delay(1000);

  // Selecciono las redes visibles del sistema de monitoreo
  while (n_iap == 0)
  {
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    logMsg(log_, "Buscando redes compatibles con el sistema");
    n = WiFi.scanNetworks();
    for (i = 0; i < n && n_iap < 255; i++)
    {
      String ssid_str = WiFi.SSID(i);
      if ( ssid_str.indexOf(ssid_key) > -1)
      {
        idx_iap[n_iap] = i;
        n_iap ++;
      }
    }
    if ( n_iap == 0 )
      displayMsg("No hay APs!");
  }

  // Seleccion manual de la red compatible
  uint8_t btn_next = 0, btn_select = 0;
  i = n_iap;
  while (!(btn_next == 0 && btn_select == 1))
  {
    if (++i >= n_iap)
      i = 0;
    display_available_networks(i, n_iap, WiFi.SSID(idx_iap[i]), WiFi.RSSI(idx_iap[i]));

    do {
      delay(500);
      btn_next = !digitalRead(PIN_FN1);
      btn_select = !digitalRead(PIN_FN0);

      // SIMULO BOTONES
      // btn_next = 0;
      // btn_select = 1;
      // delay(1500);
    } while ( !((btn_next == 0 && btn_select == 1) || (btn_next == 1 && btn_select == 0)) );
  }
  display_connection_status(WiFi.SSID(idx_iap[i]));

  return idx_iap[i];
}

// Cambia el estado del sampleo y/o despierta de deep sleep
// Tiene un filtro antirebote de 1 segundo
void toggleSampleMode()
{
  if ( millis() - t_last_touch > 1000 )
  {
    flag_sleep_mode = !flag_sleep_mode;
    t_last_touch = millis();
  }
}

void setup()
{
  // Arranco el log por puerto serie
  log_ = startLogger();

  // Prendo siempre el display
  startDisplay(true);

  pinMode(PIN_FN1, INPUT_PULLUP);
  pinMode(PIN_FN0, INPUT_PULLUP);

  // Si se despertó porque se apretó el botón entonces muestreo continuo de arranque
  if ( esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TOUCHPAD)
    flag_sleep_mode = 0;

  // Conectar la interrupción por touch capacitivo
  // El handler de interrupción podría gestionar el modo de escaneo continuo
  touchAttachInterrupt(PIN_DEEP_SLEEP_WAKEUP, toggleSampleMode, 40);

  // Establecer parámetros de conexión
  TPersistentData persistent_data;
  startEEPROM();
  readDataFromEEPROM(persistent_data);
  // Si los botones FN0 y FN1 están apretados al arrancar
  // o bien nunca se corrio la selección de red corro la rutina de selección de red
  bool flag_force_wifi_setup = (digitalRead(PIN_FN1) == 0 && digitalRead(PIN_FN0) == 0 );
  Serial.printf("Persistencia:%d\n", persistent_data.flag_paired );
  Serial.printf("PIN_FN0:%d\n", !digitalRead(PIN_FN0) );
  Serial.printf("PIN_FN1:%d\n", !digitalRead(PIN_FN1) );

  if ( flag_force_wifi_setup || persistent_data.flag_paired != 1 )
  {
    logMsg(log_, "******************************************\nDispositivo requiere configurar conexion");
    do {
      int idx_ap = selectAP();
      connectAP(WiFi.SSID(idx_ap));
    } while (waitForConnection() < 0);

    // Obtengo direccion por DHCP
    IPAddress local_ip = WiFi.localIP();
    IPAddress gateway_ip = WiFi.gatewayIP();
    IPAddress subnet = WiFi.subnetMask();

    logMsg(log_, "Parámetros de configuracion a salvarse en EEPROM: ");
    logData(log_, "Conectado a AP: ", WiFi.SSID());
    logData(log_, "Local IP: ", local_ip);
    logData(log_, "Gateway: ", gateway_ip);
    logData(log_, "Subnet Mask: ", subnet);
    for (int i = 0; i < 4; i++ )
    {
      persistent_data.local_ip[i] = local_ip[i];
      persistent_data.gateway_ip[i] = gateway_ip[i];
      persistent_data.subnet[i] = subnet[i];
      persistent_data.server_ip[i] = local_ip[i];
    }
    persistent_data.server_ip[3] = 1;

    WiFi.SSID().toCharArray(persistent_data.ssid_str, SSID_STR_SIZE);
    persistent_data.flag_paired = 1;
    writeDataToEEPROM(persistent_data);
    logMsg(log_, "******************************************\n");
  }
  else
  {
    IPAddress local_ip = IPAddress(*((uint32_t*)(persistent_data.local_ip)));
    IPAddress gateway_ip = IPAddress(*((uint32_t*)(persistent_data.gateway_ip)));
    IPAddress subnet = IPAddress(*((uint32_t*)(persistent_data.subnet)));
    WiFi.config(local_ip, gateway_ip, subnet);

    // Atención, En ESP32 WiFi.SSID no devuelve el SSID hasta que no está conectado
    // Como al llegar a este punto ya hubo una conexión previa, se la usa directamente
    // aprovechando que la gestiona internamente ESP32
    if (0 &&  WiFi.SSID() != String(persistent_data.ssid_str) )
    {
      connectAP(String(persistent_data.ssid_str));
    }
    else
    {
      logMsg(log_, "Usando configuracion WiFi almacenada");
      WiFi.reconnect();
    }
  }
  if ( !WiFi.getAutoConnect() )
    WiFi.setAutoConnect(true);

#ifdef SET_802_11G_MODE
  wifi_set_phy_mode(PHY_MODE_11G);
#endif
  server_ip = IPAddress(*((uint32_t*)(persistent_data.server_ip)));

#ifndef ANALOG_MEAS
  // Arranca la interfase digital para comunicar con sensor
  startI2CInterfase();
#endif
}

void loop()
{
  float pres_clin = 0, temp = 0;
  uint16_t sensor_status = 0;
  uint32_t t_sample;
  uint16_t nivel_bateria = 0;

  // Lo primero es tomar la medición, para asegurarme una base de tiempo estable
  t_sample = millis();

#ifdef ANALOG_MEAS
  pres_clin = float(analogRead(SMI_ANALOG_INPUT))/4095.0 * (P_max-P_min)+P_min;
#else
  sensor_status = getSensorDataI2C(&pres_clin, &temp);
#endif

  // La rutina de display se corre solo en modo de muestreo continuo
  if (flag_sleep_mode == 0 )
    displayData(pres_clin, sensor_status, WiFi.status() == WL_CONNECTED, t_sample);

  // Tomo el nivel de batería con toda la carga posible (WiFi + Sensor)
  nivel_bateria = float(analogRead(VDD_MEASURE_PIN)) / 4095.0 * 3300;

  // Cuando el nivel de batería no es el suficiente, anulo la medición y genero un status negativo
  if (nivel_bateria < MIN_BATERY_LEVEL)
  {
    pres_clin = -9999;
    sensor_status = -1;
    logData(log_, "Nivel de batería bajo: ", nivel_bateria);
    logData(log_, "Nivel Umbral: ", MIN_BATERY_LEVEL);
  }
  logData(log_, "------------------------------\nLectura del sensor. Estado", sensor_status);
  logData(log_, "Presion [mmHg clin]", pres_clin);
  logData(log_, "Temperatura [°C]", temp);

  if ( waitForConnection() == 0 && sockConnect() == 0 )
  {
    //sockSendMeasurement(sensor_status, pres_clin, temp, t_sample);
    sockSendMeasurement(sensor_status, pres_clin, nivel_bateria / 1000.0, t_sample);
    //sockSendMeasurement(0, millis()/1000.0, nivel_bateria / 1000.0, t_sample);
    client.flush();
    client.stop();
    delay(500);
  }
  else
  {
    // No pude enviar la muestra
    t_sample = 0;
  }
  cycleSleep(t_sample);
}
