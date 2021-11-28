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

#include <ESP8266WiFi.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include "user_interface.h"
#include "common.h"

#ifndef ANALOG_MEAS
#include "ps_i2c.h"
// Cuando mido por I2C dejo el ADC libre => lo puedo utilizar para sensar el nivel de batería
ADC_MODE(ADC_VCC)
#endif

int flag_sleep_mode;  // 0: no sleep o modem sleep dependiendo de Ts    1: deep sleep
int flag_sleep_mode_ant = -1;
unsigned long rtc_last_sample_time;
WiFiClient client;
IPAddress server_ip;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

int log_;
#define logData(flag,tag,value)  {if(flag){Serial.print(tag);Serial.print(": ");Serial.println(value);}}
#define logMsg(flag,msg)         {if(flag){Serial.println(msg);}}
int startLogger()
{
  int flag_debug;

  pinMode(PIN_LOG, INPUT_PULLUP);
  flag_debug = !digitalRead(PIN_LOG);
  flag_debug = 1;

  if ( flag_debug )
  {
    // Arranca el puerto serie para log de datos
    Serial.begin(LOG_DATA_SPEED_TRANSFER);
    Serial.setDebugOutput(DEBUG_OUTPUT);

    Serial.println("\n\nCliente adquisidor remoto");
    Serial.println("IAP (C) 2021\n");
  }
  return flag_debug;
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
    system_rtc_mem_write(64, &rtc_last_sample_time, 4);
    ESP.deepSleep(t_sleep * 1000);
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
    system_rtc_mem_write(64, &rtc_last_sample_time, 4);

    // Solo paso a modem-sleep si vale la pena el tiempo en que apago la antena
    if ( t_sleep > 10000 )
    {
      WiFi.disconnect();
      WiFi.forceSleepBegin();
      delay(1);
      delay(t_sleep - 2);

      WiFi.forceSleepWake();
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

  logData(log_, "Intentando conectar a Red", WiFi.SSID());
  logData(log_, "Password utilizada", password);

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

void startDisplay()
{
  pinMode(PIN_DISPLAY_VDD, OUTPUT);
  digitalWrite(PIN_DISPLAY_VDD, HIGH);

  // Inicializo el sensor en caso de estar sin inicializar
  /*if ( digitalRead(PIN_DISPLAY_VDD) == LOW )
    {
    digitalWrite(PIN_DISPLAY_VDD, HIGH);
    // Arranco comunicación con oled
    }
  */
  // Arranco comunicación con oled
  u8g2.setBusClock(100000);
  u8g2.begin();
  u8g2.clearBuffer();
  //delay(500);
}

void displayMsg(char *msg)
{
  u8g2.setPowerSave(0);
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_fub11_tr);
    u8g2.drawStr((128 - u8g2.getStrWidth(msg)) / 2, (64 + 11) / 2, msg);
  } while ( u8g2.nextPage() );
}

void displayData(float pres_clin, uint16_t sensor_status, uint32_t t_sample)
{
  char buff[20];
  static uint32_t t_sample_last = 0;
  static bool led_on = true;

  u8g2.setPowerSave(0);
  u8g2.firstPage();
  do {
    //keep this the same as it pages through the data generating the screen
    if ( sensor_status != (uint16_t) - 1 )
    {
      u8g2.setFont(u8g2_font_fub30_tr);
      sprintf(buff, "%2.1f", pres_clin);
      u8g2.drawStr((128 - u8g2.getStrWidth(buff)) / 2, 40, buff);
      u8g2.setFont(u8g2_font_fub11_tr);
      //u8g2.drawStr(0, 11, "Presion [mmHg]");
      u8g2.drawStr(0, 11, "Pr");
    }
    else
    {
      u8g2.setFont(u8g2_font_fub11_tr);
      strcpy(buff, "Error de");
      u8g2.drawStr((128 - u8g2.getStrWidth(buff)) / 2, 20, buff);
      strcpy(buff, "Sensor");
      u8g2.drawStr((128 - u8g2.getStrWidth(buff)) / 2, 20 + 22, buff);
    }
    u8g2.setFont(u8g2_font_fub11_tr);
    sprintf(buff, "Red:%s", (WiFi.status() == WL_CONNECTED ? "OK" : "NO OK"));
    u8g2.drawStr(0, 64, buff);
    if (WiFi.status() == WL_CONNECTED)
    {
      sprintf(buff, "Ts:%2.1f", ((t_sample - t_sample_last) / 1000.0));
      Serial.println(buff);
      u8g2.drawStr((128 - u8g2.getStrWidth(buff)), 64, buff);
    }
    if ( led_on )
      u8g2.drawFilledEllipse(123, 4, 4, 4, U8G2_DRAW_ALL);
    else
      u8g2.drawEllipse(123, 4, 4, 4, U8G2_DRAW_ALL);
    led_on = ! led_on;

  } while ( u8g2.nextPage() );
  t_sample_last = t_sample;
}

void startEEPROM()
{
  // commit 512 bytes of ESP8266 flash (for "EEPROM" emulation)
  // this step actually loads the content (512 bytes) of flash into
  // a 512-byte-array cache in RAM
  EEPROM.begin(sizeof(TPersistentData));
}

void writeDataToEEPROM(TPersistentData &data)
{
  // commit 512 bytes of ESP8266 flash (for "EEPROM" emulation)
  // this step actually loads the content (512 bytes) of flash into
  // a 512-byte-array cache in RAM
  EEPROM.put(0, data);
  // actually write the content of byte-array cache to
  // hardware flash.  flash write occurs if and only if one or more byte
  // in byte-array cache has been changed, but if so, ALL 512 bytes are
  // written to flash
  EEPROM.commit();
}

int readDataFromEEPROM(TPersistentData &data)
{
  // read bytes (i.e. sizeof(data) from "EEPROM"),
  // in reality, reads from byte-array cache
  // cast bytes into structure called data
  EEPROM.get(0, data);
  return data.flag_paired;
}

void connectAP(String ssid_str)
{
  logMsg(log_, "Seteando nueva configuracion de WiFi");
  if ( !WiFi.getPersistent())
    WiFi.persistent(true);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("ESP12f-ps");
  WiFi.begin(ssid_str, password);
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
  byte btn_next = 0, btn_select = 0;
  i = n_iap;
  while (!(btn_next == 0 && btn_select == 1))
  {
    if (++i >= n_iap)
      i = 0;

    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_fub11_tr);
      u8g2.setCursor(0, 15);
      u8g2.print("Red: " + String(i + 1) + "/" + String(n_iap));
      u8g2.setCursor(0, 40);
      u8g2.print(WiFi.SSID(idx_iap[i]));
      u8g2.setCursor(0, 63);
      u8g2.print(String(WiFi.RSSI(idx_iap[i])) + " dB");
    } while ( u8g2.nextPage() );

    do {
      delay(500);
      btn_next = digitalRead(PIN_DEEP_SLEEP);
      btn_select = digitalRead(PIN_FN);

      // SIMULO BOTONES
      btn_next = 0;
      btn_select = 1;
      delay(1500);
    } while ( !((btn_next == 0 && btn_select == 1) || (btn_next == 1 && btn_select == 0)) );
  }
  return idx_iap[i];
}

void setup()
{
  // Arranco el log por puerto serie
  log_ = startLogger();

  // Prendo siempre el display
  startDisplay();
  u8g2.setPowerSave(1);

  pinMode(PIN_DEEP_SLEEP, INPUT_PULLUP);
  pinMode(PIN_FN, INPUT_PULLUP);

#ifdef SOURCE_SENSOR_FROM_DO
  pinMode(PIN_SENSOR_VDD, OUTPUT);
  digitalWrite(PIN_SENSOR_VDD, LOW);
#endif

  // Establecer parámetros de conexión
  TPersistentData persistent_data;
  startEEPROM();
  readDataFromEEPROM(persistent_data);
  // Si el botón de muestreo intensivo y el de función están apretados al arrancar
  // o bien nunca se corrio la selección de red corro la rutina de selección de red
  bool flag_force_wifi_setup = (digitalRead(PIN_DEEP_SLEEP) == 0 && digitalRead(PIN_FN) == 0 );
  Serial.printf("Persistencia:%d", persistent_data.flag_paired );

  if (0)//( flag_force_wifi_setup || persistent_data.flag_paired != 1 )
  {
    logMsg(log_, "Dispositivo requiere configurar conexion");
    do {
      int idx_ap = selectAP();
      connectAP(WiFi.SSID(idx_ap));
    } while (waitForConnection() < 0);

    // Obtengo direccion por DHCP
    IPAddress local_ip = WiFi.localIP();
    IPAddress gateway_ip = WiFi.gatewayIP();
    IPAddress subnet = WiFi.subnetMask();

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
  }
  else
  {
    IPAddress local_ip = IPAddress(*((uint32_t*)(persistent_data.local_ip)));
    IPAddress gateway_ip = IPAddress(*((uint32_t*)(persistent_data.gateway_ip)));
    IPAddress subnet = IPAddress(*((uint32_t*)(persistent_data.subnet)));
    WiFi.config(local_ip, gateway_ip, subnet);

    if ( WiFi.SSID() != String(persistent_data.ssid_str) )
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
  // Se obtiene el tiempo de muestra actual usando la info guardada en la memoria del RTC en el ciclo previo
  system_rtc_mem_read(64, (void*) &rtc_last_sample_time, 4);

  // Leo siempre en el loop el estado del botón de muestreo intensivo
  flag_sleep_mode = digitalRead(PIN_DEEP_SLEEP);
  if ( flag_sleep_mode == 0 && flag_sleep_mode != flag_sleep_mode_ant )
    wifi_set_sleep_type(NONE_SLEEP_T);
  flag_sleep_mode_ant = flag_sleep_mode;

#ifdef SOURCE_SENSOR_FROM_DO
  if (!digitalRead(PIN_SENSOR_VDD))
  {
    // Alimento al sensor solo durante la medición para ahorrar energía
    digitalWrite(PIN_SENSOR_VDD, HIGH);
    delay(DELAY_SENSOR_STARTUP);
  }
#endif
#ifdef ANALOG_MEAS
  pres_clin = analogRead(A0);
#else
  sensor_status = getSensorDataI2C(&pres_clin, &temp);
#endif

  // La rutina de display se corre solo en modo de muestreo continuo
  if (flag_sleep_mode == 0 )
    displayData(pres_clin, sensor_status, t_sample);

  // Tomo el nivel de batería con toda la carga posible (WiFi + Sensor)
  // Noté también que hay un sesgo de 0.322V en la medición
  nivel_bateria = ESP.getVcc() + 322;

#ifdef SOURCE_SENSOR_FROM_DO
  if (flag_sleep_mode == 1) // Apago la alimentación del sensor
    digitalWrite(PIN_SENSOR_VDD, LOW);
#endif

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
