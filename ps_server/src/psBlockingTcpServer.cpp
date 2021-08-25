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

#include "psBlockingTcpServer.h"
#include <QTcpSocket>
#include <QFile>

#define SOCKET_TIMEOUT 10000

psBlockingTcpServer::psBlockingTcpServer(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void psBlockingTcpServer::startServer(int port)
{
    if(!tcpServer->listen(QHostAddress::Any, port))
        qDebug() << "No puedo arrancar servidor\nProbablemente el puerto esté ocupado";
    else
        qDebug() << "Escuchando en puerto " << port << "...";
}

void psBlockingTcpServer::newConnection()
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    int socketDescriptor = socket->socketDescriptor();

    if( socket->waitForReadyRead(SOCKET_TIMEOUT) )
    {
        int16_t read_status;
        double pressure_clin,temperature;
        quint64 delta_sample_time_wemos;

        // Leo el stream
        QByteArray data_received = socket->readAll();
        QList<QByteArray> data_array = data_received.split(';');
        qDebug() << "Datos recibidos por socket" << data_received;
        if( data_array.length()==4 )
        {
            read_status = data_array.at(0).toUInt();
            pressure_clin = data_array.at(1).toDouble();
            temperature = data_array.at(2).toDouble();
            delta_sample_time_wemos = data_array.at(3).toULongLong();
        }
        QDateTime current_time = QDateTime::currentDateTime();
        if( read_status!=-1 )
        /// TODO: todavia no se como interpretar el status del sensor
        {
            qDebug() << "Mediciones válidas: " << pressure_clin << "mmHg clin   a temperatura: " << temperature;
            // El problema de los 2 relojes. Asumo que el jitter no es tan grave cuando la conexion es buena
            // y tomo el tiempo de muestreo desde la PC, con una base de tiempo más estable
            QDateTime sample_time = current_time.addMSecs(-delta_sample_time_wemos);

            emit sampleArrived(socketDescriptor,pressure_clin,sample_time);
            emit sensorStatus(socketDescriptor,0,sample_time);

            QFile file("datos.csv");
            if( file.open(QFile::Append))
            {
                QTextStream output(&file);
                output << sample_time.toString("dd/MM/yyyy hh:mm:ss") << ";" << QString::number(sample_time.toSecsSinceEpoch()) << ";" <<  QString::number(pressure_clin) << ";" <<QString::number(temperature) << ";" << QString::number(delta_sample_time_wemos) << ";" << QString::number(read_status) << "\n";
            }
        }
        else
        {
            qDebug() << "Error de sensor -> Medición inválida ";
            emit sensorStatus(socketDescriptor,-1,current_time);
        }        
    }
    else
        emit sensorStatus(socketDescriptor,-2,QDateTime::currentDateTime());
    socket->disconnectFromHost();
}
