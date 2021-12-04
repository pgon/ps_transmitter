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

#include <Wire.h>
#include <HardwareSerial.h>

#include "ps_i2c.h"
#include "common.h"

//#define DEBUG_SMI_I2C


void startI2CInterfase()
{
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_CLOCK);
}


uint16_t getSensorDataI2C(float *pres_clin, float *temp)
{
  uint8_t error;
  char msg_received[10];

  *pres_clin = 0;
  *temp = 0;

  // Según el protocolo (pag. 9 manual instrucciones), se marca un start y se transmite
  // la dirección de memoria a leer, y finalmente se marca un restart
  // Por lo visto para tomar los datos esta dirección siempre es 0x2E (temp) 0x30 (presion) 0x32 (status).
  // Solo se indica la primera y luego el sensor envía los 3 datos.
  Wire.beginTransmission(address);
  Wire.write(0x2E);
  error = Wire.endTransmission(false);

  if (error != 0)
  {
#ifdef DEBUG_SMI_I2C
    Serial.print("(SMI I2C) -> Esclavo NO encontrado\n");
#endif
    return -1;
  }
  else
  {
#ifdef DEBUG_SMI_I2C
    Serial.print("(SMI I2C) -> Esclavo encontrado\n");
#endif
  }

  // Luego del pedido de lectura, se marca la escritura del slave (esto le indica que tiene que mandar las posiciones de memoria)
  // y se codifica en el bus como una escritura a (0x6D<<1|1)
  // Según el protocolo se esperan 6 bytes
  uint8_t n_return_bytes = Wire.requestFrom(address, smi_nb_expected);
#ifdef DEBUG_SMI_I2C
  Serial.printf("(SMI I2C) -> Se solicitan %d bytes y retorna %d\n", smi_nb_expected, n_return_bytes);
#endif


#ifdef DEBUG_SMI_I2C
  Serial.print("(SMI I2C) -> Leyendo el buffer de datos\n");
#endif
  int i = 0;
  uint8_t b;

  while (Wire.available())
  {
    b = Wire.read();
#ifdef DEBUG_SMI_I2C
    Serial.print(b, HEX);
    Serial.print(" ");
#endif
    msg_received[i++] = b;
  }

  // Luego de recibir los datos se marca la finalización de la transmisión
  // Si hubo error a nivel I2C se marca la lectura como errónea indicando a la capa superior
  // que no puede usarla
  Wire.endTransmission();
  if ( smi_nb_expected != i )
  {
#ifdef DEBUG_SMI_I2C
    Serial.print("(SMI I2C) -> Error canal comunicacion I2C. Se recibe cantidad diferente a la esperada\n");
#endif
    return -1;
  }

  // Asignación de registros según los datos leídos
  uint16_t DSP_T_L =  (uint16_t) msg_received[0];
  uint16_t DSP_T_H =  (uint16_t) msg_received[1];

  uint16_t DSP_S_L =  (uint16_t) msg_received[2];
  uint16_t DSP_S_H =  (uint16_t) msg_received[3];

  uint16_t STATUS_L =  (uint16_t) msg_received[4];
  uint16_t STATUS_H =  (uint16_t) msg_received[5];

  // Se arman los registros de 16 bits
  uint16_t STATUS = STATUS_H << 8 | STATUS_L;
  uint16_t DSP_T = DSP_T_H << 8 | DSP_T_L; // TODO: Falta seber cómo se pasa de este número a una verdadera medición de temperatura (40AN7000)
  uint16_t DSP_S = DSP_S_H << 8 | DSP_S_L;

#ifdef DEBUG_SMI_I2C
  Serial.print("\n(SMI I2C) -> STATUS SENSOR=");
  Serial.println(STATUS, HEX);
#endif
  //if ( STATUS != 0xC018 ) // TODO: Falta chequear el status cuando está todo OK!
  //  return -1;

  // Se calcula la lectura de presión clin
  float sensor_counts;
  if ( DSP_S & 0x4000 ) // Si el bit Z está prendido, el número es negativo => hago la conversión por complemento 2
  {
    sensor_counts =  (0xFFFF & (~DSP_S | 0x1));
    sensor_counts *= -1;
  }
  else
    sensor_counts = DSP_S;

  // Escalamiento de la medición (fórmula manual de usuario pag. 10)
  *temp = (float) DSP_T;

  float P_read_abs = P_min + ((sensor_counts + 26215) / 52429) * (P_max - P_min);
  *pres_clin = P_read_abs - 760;

  return STATUS;
}


void scanI2CBus()
{
  uint8_t error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    // The i2c scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println(" !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("Done.\n");
  }

  delay(2000);
}
