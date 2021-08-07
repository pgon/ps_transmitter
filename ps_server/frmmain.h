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

#ifndef FRMMAIN_H
#define FRMMAIN_H
#include "psTcpServer.h"
#include "psBlockingTcpServer.h"
#include "trendingThread.h"


#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class frmMain; }
QT_END_NAMESPACE



class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    frmMain(QWidget *parent = nullptr);
    ~frmMain();

private:
    Ui::frmMain *ui;    
    psBlockingTcpServer *psAdq;
    //psTcpServer *psAdq;
    trendingThread *trending;

    QTimer *watchdog_sensor;
    QPixmap led_red, led_green;



private slots:
    void on_btnSalir_clicked();
    void logSample(int socket_src, double sample_value, QDateTime sample_time);
    void adqAlarm(int socket_src, int alarm_status, QDateTime sample_time);
    void sensorCommunicationTimeout();
};
#endif // FRMMAIN_H
