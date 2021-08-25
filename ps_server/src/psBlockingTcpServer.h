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

#ifndef PSBLOCKINGTCPSERVER_H
#define PSBLOCKINGTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QDateTime>

class psBlockingTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit psBlockingTcpServer(QObject *parent = nullptr);
    void startServer(int port);

signals:
    void sampleArrived(int socket_src,double sample_value,QDateTime sample_time);
    void sensorStatus(int socket_src,int sensor_status,QDateTime sample_time);

private slots:
    void newConnection();

private:
    QTcpServer *tcpServer = nullptr;

};

#endif // PSBLOCKINGTCPSERVER_H
