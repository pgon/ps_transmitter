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

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "common.h"

// Inicia comunicación con oled
void startDisplay(bool flag_powersave);

// Setea modo de energía del display oled
void displaySetPowerSave(bool flag_powersave);

// Muestra un mensaje en display oled
void displayMsg(char *msg);

// Muestra los datos de la medición en display oled
void displayData(float pres_clin, uint16_t sensor_status, int wifi_connected, uint32_t t_sample);

// Muestra los datos de una de las redes disponibles
void display_available_networks(int index, int n_iap, String ssid, long rssi);

// Muestra el progreso de la conexión a una red
void display_connection_status(String ssid);

#endif
