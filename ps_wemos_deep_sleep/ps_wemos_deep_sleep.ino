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
#include "user_interface.h"
#include "common.h"

#ifndef ANALOG_MEAS
#include "ps_i2c.h"
// Cuando mido por I2C dejo el ADC libre => lo puedo utilizar para sensar el nivel de batería
ADC_MODE(ADC_VCC)
#endif

int flag_sleep_mode;  // 0: no sleep o modem sleep dependiendo de Ts    1: deep sleep
unsigned long rtc_last_sample_time;
WiFiClient client;

int log_;
#define logData(flag,tag,value)  {if(flag){Serial.print(tag);Serial.print(": ");Serial.println(value);}}
#define logMsg(flag,msg)         {if(flag){Serial.println(msg);}}
int startLogger()
{
  int flag_debug;

  pinMode(PIN_LOG, INPUT_PULLUP);
  flag_debug = !digitalRead(PIN_LOG);
  
  if ( flag_debug )
  {
    // Arranca el puerto serie para log de datos
    Serial.begin(LOG_DATA_SPEED_TRANSFER);
    Serial.println("\n\nCliente adquisidor remoto");
    Serial.println("IAP (C) 2021\n");
  }
  return flag_debug;
}

void cycleSleep(unsigned long t_sample)
{
  long t_sleep;

  if (flag_sleep_mode) // Deep sleep
  {    
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
    t_sleep = SAMPLE_TIME - (millis() - t_sample);
    if ( t_sleep < 0 )
      t_sleep = SAMPLE_TIME;
    logData(log_, "Durmiendo [ms]", t_sleep);
    rtc_last_sample_time += millis() + t_sleep;
    system_rtc_mem_write(64, &rtc_last_sample_time, 4);

    // Solo paso a modem-sleep si vale la pena el tiempo en que apago la antena
    if ( t_sleep > 10 )
    {
      WiFi.disconnect();
      WiFi.forceSleepBegin();
      delay(1);
      delay(t_sleep - 2);

      WiFi.forceSleepWake();
      delay(1);
    }
  }
}

int wifiConnect()
{
  logData(log_, "Intentando conectar a Red", ssid);
  logData(log_, "Password utilizada", password);

  // Establecer parámetros de conexión
  // WiFi.persistent( false );
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int retry_counter_wifi = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    retry_counter_wifi++;
    if ( retry_counter_wifi >= NB_RETRY_WIFI )
    {
      logMsg(log_, "Imposible conectar a red WiFi");
      return -1;
    }
  }

  if ( flag_sleep_mode == 0 )
    wifi_set_sleep_type(NONE_SLEEP_T);

  logData(log_, "Conectado a WiFi con IP", WiFi.localIP());
  return 0;
}

int sockConnect()
{
  int retry_counter_socket = 0;

  logData(log_, "Intento conectar con host", host);
  logData(log_, "puerto", port);

  while (!client.connect(host, port))
  {
    retry_counter_socket++;
    if ( retry_counter_socket >= NB_RETRY_SOCKET )
    {
      logMsg(log_, "Imposible conectar con Host");
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
  client.stop();
  delay(500);
}

void setup()
{
  log_ = startLogger();

  pinMode(PIN_DEEP_SLEEP, INPUT_PULLUP);
  pinMode(PIN_SENSOR_VDD, OUTPUT);

#ifndef ANALOG_MEAS
  // Arranca la interfase digital para comunicar con sensor
  startI2CInterfase();
#endif
}

void loop()
{
  float pres_clin = 0, temp = 0;
  uint16_t sensor_status = 0;
  unsigned long t_sample;
  uint16_t nivel_bateria = 0;

  // Lo primero es tomar la medición, para asegurarme una base de tiempo estable
  t_sample = millis();
  // Se obtiene el tiempo de muestra actual usando la info guardada en la memoria del RTC en el ciclo previo
  system_rtc_mem_read(64, (void*) &rtc_last_sample_time, 4);

#ifdef ANALOG_MEAS
  pres_clin = analogRead(A0);
#else
  // Alimento al sensor solo durante la medición para ahorrar energía
  digitalWrite(PIN_SENSOR_VDD,HIGH);
  delay(DELAY_SENSOR_STARTUP);  

  sensor_status = getSensorDataI2C(&pres_clin, &temp);
  nivel_bateria = ESP.getVcc();

  // Apago la alimentación del sensor
  digitalWrite(PIN_SENSOR_VDD,LOW);  
#endif
  logData(log_, "------------------------------\nLectura del sensor. Estado", sensor_status);
  logData(log_, "Presion [mmHg clin]", pres_clin);
  logData(log_, "Temperatura [°C]", temp);

  flag_sleep_mode = digitalRead(PIN_DEEP_SLEEP);
  // TODO: ESTOY FORZANDO ACA EL SLEEP MODE porque no puedo conectar otro cable entre D7 y GND
  //flag_sleep_mode = 0;

  if ( WiFi.status() != WL_CONNECTED )
    wifiConnect();
  if ( WiFi.status() == WL_CONNECTED && sockConnect() == 0 )
    //sockSendMeasurement(sensor_status, pres_clin, temp, t_sample);
    sockSendMeasurement(sensor_status, pres_clin, nivel_bateria/1000.0, t_sample);
  cycleSleep(t_sample);
}
