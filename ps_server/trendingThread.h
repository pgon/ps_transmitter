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

#ifndef TRENDINGTHREAD_H
#define TRENDINGTHREAD_H

#include <QObject>
#include <QThread>
#include <qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <QDateTime>
#include <QTimer>
#include <QMutex>

#define N_SAMPLES 100

class trendingThread : public QThread
{
    Q_OBJECT

private:
    QMutex mutex;
    QwtPlot* plotter_;
    QwtPlotCurve *curve;

    QTimer *refresh_timer;
    QDateTime last_refresh;

private slots:
    void refreshPlotter();

public slots:
    void appendSample(int socket_src, double sample_value, QDateTime sample_time);

public:
    trendingThread(QwtPlot *plotter, QObject *parent = 0);

    int plotDataSize;
    QDateTime last_sample_time;
    int n_samples;
    float autonomia_promedio;

    QDateTime sample_time_local = QDateTime::currentDateTime();
    QDateTime last_sample_time_local = QDateTime::currentDateTime();
    quint64 wemos_last_sample_time =0;

    // data arrays for the plot
    double xData[N_SAMPLES];
    double yData[N_SAMPLES];

};

#endif // TRENDINGTHREAD_H
