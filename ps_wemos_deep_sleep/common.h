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

#define PIN_DISPLAY_VDD           D3
#define PIN_FN                    D5

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
//#define SOURCE_SENSOR_FROM_DO
#ifdef SOURCE_SENSOR_FROM_DO
#define PIN_SENSOR_VDD            D3
// El sensor necesita un tiempo hasta dar una medición buena
// Si se interrumpe la alimentación como parte de la estrategia de ahorro de energía
// no se debería leer inmediatamente después de reponerla
// Se observó que para 500ms se tiene un sesgo de 3mmHg
// Luego se observa que para valores mayores a 2seg, se converge a una lectura dentro de un desvío <1mmHg
#define DELAY_SENSOR_STARTUP      2000
#endif


/* Parámetros de la consola de logs por medio del puerto serie
*/
#define LOG_DATA_SPEED_TRANSFER   115200
#define PIN_LOG                   D4
#define DEBUG_OUTPUT              false


/* Si quiero solo lecturas analógicas, descomentar la siguiente línea
   Atención, así se pierde el monitoreo de la batería
*/
//#define ANALOG_MEAS


/* Parámetros de ahorro de energía
    Si el pin D7 se pone a GND, entonces muestrea sin ir a deep sleep con una tasa dada por SAMPLE_TIME
    Si en cambio no se introduce ese jumper, va a deep sleep con tasa de muestreo SAMPLE_TIME_DEEP_SLEEP
*/
#define PIN_DEEP_SLEEP            D7
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
    byte flag_paired;
    char ssid_str[SSID_STR_SIZE] = "";
    byte local_ip[4];
    byte gateway_ip[4];
    byte subnet[4];
    byte server_ip[4];
  } TPersistentData;

#endif
