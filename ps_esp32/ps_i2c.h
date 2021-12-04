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

#ifndef PS_I2C_H_
#define PS_I2C_H_

#include "common.h"

const uint8_t address = 0x6C;
const uint8_t smi_nb_expected = 6;

/* void startI2CInterfase()
 * 
 *  Función para arrancar la comunicación I2C en el WEMOS
 */
void startI2CInterfase();

/* int getSensorDataI2C(float *pres_clin, float *temp)
 *  
 *  Realiza una lectura del sensor. Registra presión relativa a la atmosférica y temperatura
 *  Devuelve el status de la medición 
 */
uint16_t getSensorDataI2C(float *pres_clin, float *temp);

/* void scanI2CBus()
 *  
 *  Función para scanear el bus I2C en busca de dispositivos conectados 
 *  El sensor tiene una dirección establecida de fábrica en 0x6C/0x6D siguiendo los lineamientos del protocolo
 */
void scanI2CBus();


#endif 
