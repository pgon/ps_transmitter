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


/* Datos del sensor a ser tomados de la hoja de datos
   La idea es programar un WEMOS para cada sensor y que estos queden hermanados
   Si bien hasta cierto punto la intercambiabilidad no es problema, surgen cuestiones
   con ciertos parámetros que dependerían del sensor
*/
const float P_max = 1260;
const float P_min = 460;

/* Gestión de alimentación del sensor
*/
#define PIN_SENSOR_VDD            D3
#define MIN_BATERY_LEVEL          2800
// El sensor necesita un tiempo hasta dar una medición buena
// Si se interrumpe la alimentación como parte de la estrategia de ahorro de energía
// no se debería leer inmediatamente después de reponerla
// Se observó que para 500ms se tiene un sesgo de 3mmHg
// Luego se observa que para valores mayores a 2seg, se converge a una lectura dentro de un desvío <1mmHg
#define DELAY_SENSOR_STARTUP      2000


/* Parámetros de la consola de logs por medio del puerto serie
*/
#define LOG_DATA_SPEED_TRANSFER   115200
#define PIN_LOG                   D5
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
// LUEGO DEJAR ESTO EN 2 SEGUNDOS POR EJEMPLO. AHORA ESTOY PROBANDO LA AUTONOMIA SIN DESCONECTAR WIFI PERO MANDANDO ESPACIADO
#define SAMPLE_TIME               2000


/* Parámetros de conexion por WiFi
*/
#define PRUEBA_TELERED

/*
  #define ssid              "ppp"
  #define password          "010101010101"
  #define host              "10.42.0.1"
*/


/*#define ssid              "iap"
  #define password          "lucas1234"
  #define host              "192.168.137.1"
*/

#ifdef PRUEBA_TELERED
#define ssid              "TeleRed-9724"
#define password          "010101010101"
//#define SET_802_11G_MODE

#define STATIC_ADRESS
#ifdef STATIC_ADRESS
#define sta_address       {192,168,0,80}
#define sta_gateway       {192,168,0,13}
#define sta_netmask       {255,255,255,0}
#endif
#define host              "192.168.0.13"
#endif

#ifdef PRUEBA_CARRASCO
#define ssid              "CARRASCO 2.4"
#define password          ""
//#define SET_802_11G_MODE

#define STATIC_ADRESS
#ifdef STATIC_ADRESS
#define sta_address       {192,168,0,80}
#define sta_gateway       {192,168,0,79}
#define sta_netmask       {255,255,255,0}
#endif
#define host              "192.168.0.79"
#endif

#define port              9999
#define NB_RETRY_WIFI     20             // espera de la conexión a WiFi = NB_RETRY_WIFI*500ms
#define NB_RETRY_SOCKET   5              // reintentos para conexión a host

#endif
