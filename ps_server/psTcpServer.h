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

#ifndef PSTCPSERVER_H
#define PSTCPSERVER_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

class serverThread : public QThread
{
    Q_OBJECT
public:
    QTcpSocket *socket;
    explicit serverThread(qintptr ID, QObject *parent = 0);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void dataRead(int socket_src,double sample_value,QDateTime sample_time);
    void sensorStatus(int socket_src,int sensor_status,QDateTime sample_time);

public slots:
    void readyRead();
    void disconnected();
    void timeoutCheck();

private:
    qintptr socketDescriptor;
    QTimer *watchdog_timer;
};


class psTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit psTcpServer(QObject *parent = 0);
    void startServer(int port);

signals:
    void sampleArrived(int socket_src,double sample_value,QDateTime sample_time);
    void sensorStatus(int socket_src,int sensor_status,QDateTime sample_time);

public slots:


private:

protected:
    void incomingConnection(qintptr socketDescriptor);
};

#endif // PSTCPSERVER_H
