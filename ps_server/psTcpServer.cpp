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

#include "psTcpServer.h"
#include <QDataStream>

#define THREAD_TIMEOUT 10000

serverThread::serverThread(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
}

void serverThread::run()
{
    // thread starts here
    qDebug() << " Thread started";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        qDebug() << "(" << socketDescriptor << ")" << " Error al abrir socket";
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected";

    watchdog_timer = new QTimer();
    connect(watchdog_timer, SIGNAL(timeout()), this, SLOT(timeoutCheck()));
    watchdog_timer->setSingleShot(true);
    watchdog_timer->start(THREAD_TIMEOUT);

    exec();
}

void serverThread::timeoutCheck()
{
    qDebug() << "(" << socketDescriptor << ")" << " No está respondiendo. Intentando cerrar conexión con Host ... ";
//    socket->disconnectFromHost();
    socket->abort();
}

void serverThread::readyRead()
{
    // Llegó la nueva muestra => reseteo el watchdog
    watchdog_timer->start(THREAD_TIMEOUT);

    int read_status=-1;
    double pressure_clin,temperature;
    quint64 delta_sample_time_wemos;

    // Leo el stream
    QByteArray data_received = socket->readAll();
    QList<QByteArray> data_array = data_received.split(';');
    qDebug() << "Datos recibidos por socket" << data_received;
    if( data_array.length()==4 )
    {
        read_status = data_array.at(0).toInt();
        pressure_clin = data_array.at(1).toDouble();
        temperature = data_array.at(2).toDouble();
        delta_sample_time_wemos = data_array.at(3).toULongLong();
    }
    QDateTime current_time = QDateTime::currentDateTime();
    if( read_status==0 )
    {
        qDebug() << "Mediciones válidas: " << pressure_clin << "mmHg clin   a temperatura: " << temperature;
        // El problema de los 2 relojes. Asumo que el jitter no es tan grave cuando la conexion es buena
        // y tomo el tiempo de muestreo desde la PC, con una base de tiempo más estable
        QDateTime sample_time = current_time.addMSecs(-delta_sample_time_wemos);

        emit dataRead(socketDescriptor,pressure_clin,sample_time);
        emit sensorStatus(socketDescriptor,0,sample_time);
    }
    else
    {
        qDebug() << "Error de sensor! -> Medición inválida ";
        emit sensorStatus(socketDescriptor,-1,current_time);
    }
}

void serverThread::disconnected()
{
    qDebug() << "(" << socketDescriptor << ")" << " Desconectado";
    socket->deleteLater();
    exit(0);
}


psTcpServer::psTcpServer(QObject *parent) :
    QTcpServer(parent)
{
}

void psTcpServer::startServer(int port)
{
    if(!this->listen(QHostAddress::Any, port))
        qDebug() << "No puedo arrancar servidor";
    else
        qDebug() << "Escuchando en puerto " << port << "...";
}

// This function is called by QTcpServer when a new connection is available.
void psTcpServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Conectando ...";

    // Every new connection will be run in a newly created thread
    serverThread *thread = new serverThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread,SIGNAL(finished()), thread, SLOT(deleteLater()));
    //connect(thread,SIGNAL(dataRead(int,double,quint64)),(trendingThread*)trending_,SLOT(showSample(double,quint64)));
    connect(thread,SIGNAL(dataRead(int,double,QDateTime)),this,SIGNAL(sampleArrived(int,double,QDateTime)));
    connect(thread,SIGNAL(sensorStatus(int,int,QDateTime)),this,SIGNAL(sensorStatus(int,int,QDateTime)));
    thread->start();
}


