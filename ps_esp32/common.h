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

#ifndef COMMON_H_
#define COMMON_H_

#include <Arduino.h>

/*  Mapeo de los botones
 */
#define PIN_FN0                   32
#define PIN_FN1                   33


/* Configuración de protocolo I2C  
 */
#define I2C_SCL_PIN    19
#define I2C_SDA_PIN    23
#define I2C_CLOCK      50000


/* Datos del sensor a ser tomados de la hoja de datos
   La idea es programar un WEMOS para cada sensor y que estos queden hermanados
   Si bien hasta cierto punto la intercambiabilidad no es problema, surgen cuestiones
   con ciertos parámetros que dependerían del sensor
*/
const float P_max = 1260;
const float P_min = 460;

/* Gestión de alimentación del sensor
*/
#define MIN_BATERY_LEVEL          2000
#define VDD_MEASURE_PIN           34

/* Parámetros de la consola de logs por medio del puerto serie
*/
#define LOG_DATA_SPEED_TRANSFER   115200
#define PIN_LOG                   24
#define DEBUG_OUTPUT              false

extern int log_;
#define logData(flag,tag,value)  {if(flag){Serial.print(tag);Serial.print(": ");Serial.println(value);}}
#define logMsg(flag,msg)         {if(flag){Serial.println(msg);}}
int startLogger();


/* Si quiero solo lecturas analógicas, descomentar la siguiente línea
   Atención, así se pierde el monitoreo de la batería
*/
//#define ANALOG_MEAS
#define SMI_ANALOG_INPUT          35

/* Parámetros de ahorro de energía
    Si el pin D7 se pone a GND, entonces muestrea sin ir a deep sleep con una tasa dada por SAMPLE_TIME
    Si en cambio no se introduce ese jumper, va a deep sleep con tasa de muestreo SAMPLE_TIME_DEEP_SLEEP
*/
#define PIN_DEEP_SLEEP_WAKEUP     T3
#define SAMPLE_TIME_DEEP_SLEEP    30000
#define SAMPLE_TIME               2000


/* Parámetros de conexion por WiFi
*/
#define password          "lucas1234"
#define ssid_key          "iap"
#define port              9999
#define NB_RETRY_WIFI     20             // espera de la conexión a WiFi = NB_RETRY_WIFI*500ms
#define NB_RETRY_SOCKET   5              // reintentos para conexión a host


#define SSID_STR_SIZE 20
typedef struct { 
    uint8_t flag_paired;
    char ssid_str[SSID_STR_SIZE] = "";
    uint8_t local_ip[4];
    uint8_t gateway_ip[4];
    uint8_t subnet[4];
    uint8_t server_ip[4];
  } TPersistentData;

#endif
