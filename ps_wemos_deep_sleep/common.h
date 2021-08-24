/***************************************************************************
 * Copyright (C) 2021 by Pablo Gonzalez <pgonzal@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_


/* Datos del sensor a ser tomados de la hoja de datos
 * La idea es programar un WEMOS para cada sensor y que estos queden hermanados 
 * Si bien hasta cierto punto la intercambiabilidad no es problema, surgen cuestiones 
 * con ciertos parámetros que dependerían del sensor
 */
const float P_max = 1260;
const float P_min = 460;

/* Gestión de alimentación del sensor
 */
#define PIN_SENSOR_VDD            D3
#define MIN_BATERY_LEVEL          28000
// El sensor necesita un tiempo hasta dar una medición buena
// Si se interrumpe la alimentación como parte de la estrategia de ahorro de energía 
// no se debería leer inmediatamente después de reponerla
// Se observó que para 500ms se tiene un sesgo de 3mmHg
// Luego se observa que para valores mayores a 2seg, se converge a una lectura dentro de un desvío <1mmHg
#define DELAY_SENSOR_STARTUP      2000


/* Parámetros de la consola de logs por medio del puerto serie  
 */
#define LOG_DATA_SPEED_TRANSFER   9600
#define PIN_LOG                   D5


// Si quiero solo lecturas analógicas, descomentar la siguiente línea
//#define ANALOG_MEAS


/* Parámetros de ahorro de energía
 *  Si el pin D7 se pone a GND, entonces muestrea sin ir a deep sleep con una tasa dada por SAMPLE_TIME
 *  Si en cambio no se introduce ese jumper, va a deep sleep con tasa de muestreo SAMPLE_TIME_DEEP_SLEEP
 */
#define PIN_DEEP_SLEEP            D7
#define SAMPLE_TIME_DEEP_SLEEP    30000
// LUEGO DEJAR ESTO EN 2 SEGUNDOS POR EJEMPLO. AHORA ESTOY PROBANDO LA AUTONOMIA SIN DESCONECTAR WIFI PERO MANDANDO ESPACIADO
#define SAMPLE_TIME               2000


/* Parámetros de conexion por WiFi
 */
/*
#define ssid              "TeleRed-9724"
#define password          "010101010101"
#define host              "192.168.0.13"
*/

/*
#define ssid              "ppp"
#define password          "010101010101"
#define host              "10.42.0.1"
*/
#define ssid              "iap"
#define password          "lucas1234"
#define host              "192.168.137.1"

#define port              9999
#define NB_RETRY_WIFI     20             // cantidad de reintentos de conexion a WiFi
#define NB_RETRY_SOCKET   5

#endif
