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

#include "display.h"
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C  u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ I2C_SCL_PIN, /* data=*/ I2C_SDA_PIN);

void startDisplay(bool flag_powersave)
{
  u8g2.setBusClock(I2C_CLOCK);
  u8g2.begin();
  u8g2.clearBuffer();
  if (flag_powersave)
    displaySetPowerSave(flag_powersave);
}

void displaySetPowerSave(bool flag_powersave)
{
  u8g2.setPowerSave(flag_powersave);
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


void displayData(float pres_clin, uint16_t sensor_status, int wifi_connected, uint32_t t_sample)
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
    sprintf(buff, "Red:%s", (wifi_connected ? "OK" : "NO OK"));
    u8g2.drawStr(0, 64, buff);
    if (wifi_connected)
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


void display_available_networks(int index, int n_iap,String ssid,long rssi)
{
  u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_fub11_tr);
      u8g2.setCursor(0, 15);
      u8g2.print("Red: " + String(index + 1) + "/" + String(n_iap));
      u8g2.setCursor(0, 40);
      u8g2.print(ssid);
      u8g2.setCursor(0, 63);
      u8g2.print(rssi);
    } while ( u8g2.nextPage() );
}


void display_connection_status(String ssid)
{
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_fub11_tr);
    u8g2.setCursor(0, 15);
    u8g2.print("Seleccionada:");
    u8g2.setCursor(0, 40);
    u8g2.print(ssid);
    u8g2.setCursor(0, 63);
    u8g2.print("Conectando ...");
  } while ( u8g2.nextPage() );
}
