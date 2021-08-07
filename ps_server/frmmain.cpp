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


#include "frmmain.h"
#include "ui_frmmain.h"
#include <QtNetwork>
#include "psTcpServer.h"

#define COMMUNICATION_TIMEOUT   40000
#define TCP_PORT                9999

frmMain::frmMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::frmMain)
{
    ui->setupUi(this);

    led_red.load(":/led_r");
    led_green.load(":/led_v");
    ui->lblSensorStatus->setPixmap(led_red);

    trending = new trendingThread(ui->trendViewer,this);

    //psAdq = new psTcpServer(this);
    psAdq = new psBlockingTcpServer(this);
    connect(psAdq,SIGNAL(sampleArrived(int,double,QDateTime)),trending,SLOT(appendSample(int,double,QDateTime)));
    connect(psAdq,SIGNAL(sampleArrived(int,double,QDateTime)),this,SLOT(logSample(int,double,QDateTime)));
    connect(psAdq,SIGNAL(sensorStatus(int,int,QDateTime)),this,SLOT(adqAlarm(int,int,QDateTime)));
    psAdq->startServer(TCP_PORT);

    // Armo un watchdog para controlar cuando me dejan de llegar muestras
    watchdog_sensor = new QTimer();
    connect(watchdog_sensor, SIGNAL(timeout()), this, SLOT(sensorCommunicationTimeout()));
    watchdog_sensor->start(COMMUNICATION_TIMEOUT);
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::logSample(int socket_src, double sample_value, QDateTime sample_time)
{
    QString log_data = "(socket: " + QString::number(socket_src)+") tiempo= " +sample_time.toString()+ " valor= "+QString::number(sample_value,'f',2);
    ui->logger->append(log_data);
}

void frmMain::sensorCommunicationTimeout()
{
    ui->lblSensorStatus->setPixmap(led_red);
    QString log_data = "tiempo= " +QDateTime::currentDateTime().toString()+ " ERROR DE COMUNICACION! ";
    ui->logger->append(log_data);
}

void frmMain::adqAlarm(int socket_src, int alarm_status, QDateTime sample_time)
{
    // Reseteo el watchdog de la comuniación, porque si pasé por acá es porque tenía una activa
    watchdog_sensor->start(COMMUNICATION_TIMEOUT);

    // Actualizo la visualización de la alarma
    if(alarm_status==0)
        ui->lblSensorStatus->setPixmap(led_green);
    else
        ui->lblSensorStatus->setPixmap(led_red);

    if( alarm_status==-1 )
    {
        QString log_data = "(socket: " + QString::number(socket_src)+") tiempo= " +sample_time.toString()+ " ERROR DEL SENSOR";
        ui->logger->append(log_data);
    }
    if( alarm_status==-2 )
    {
        QString log_data = "(socket: " + QString::number(socket_src)+") tiempo= " +sample_time.toString()+ " TIMEOUT EN SOCKET";
        ui->logger->append(log_data);
    }
}


void frmMain::on_btnSalir_clicked()
{
    delete psAdq;
    this->close();
}
