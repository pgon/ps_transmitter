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

#include "common.h"

int log_;

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
