/***************************************************************************
 * Copyright (C) 2016 by Pablo Gonzalez <pgonzal@gmail.com>
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


#ifndef QTREND_H
#define QTREND_H

#include <QTime>
#include <QVector>
#include <QMutex>
#include <QVariant>

#define SPAN_SAMPLE_SIZE    500
#define MAX_SPAN_SIZE   (8*60*60)

class QTrend
{
private:
    QMutex _trend_mutex;
    QString _tag_name;
    float _min_value;
    float _max_value;
    QString _axis_label;

    float _time_base;                       // Base de tiempo para el muestreo en ms. Debe ser divisor de 1000
    int _time_base_factor;                  // Multiplicador de la base de tiempo para tener 1000ms
    int _n_sample;

    // Vectores de almacenamiento con un periodo de 1 segundo
    // Almacenan un periodo de 8 horas => 28800 muestras
    // Así al tomar la ventana más chica de 10 minutos tengo al menos 600 muestras
    QVector<qint64> _time;
    QVector<float> _value;

    double _trendingPenX[SPAN_SAMPLE_SIZE];    // trendline calculada a partir de los datos originales para el span deseado
    double _trendingPenY[SPAN_SAMPLE_SIZE];    // trendline calculada a partir de los datos originales para el span deseado
    qint64 _trendingSpan;                      // Tamaño de la ventana expresada en ms
    double _trendingTimeScale;                 // Escala de visualizacion de trendingPenX
    qint64 _last_sample_timestamp;            // Timestamp de la ultima muestra de la ventana
    qint64 _last_sample_deltaT;

    // Para manejar trends de tiempo real, debo almacenar más rápido (4 veces por segundo)
    double _realTimeVectorX[SPAN_SAMPLE_SIZE];    // trendline realtime: se almacena un vector fijo con los ultimos SPAN_SAMPLE_SIZE. A diferencia del otro pen, se conservan las muestras a 250ms solo durante 2 minutos (@ BT=250ms)
    double _realTimeVectorY[SPAN_SAMPLE_SIZE];
    double _realTimeSpan;                     // Por ejemplo a 250ms por muestra, equivale a poco más de 2minutos (@ BT=250ms)

    double findSampleValue(int &idxStart,qint64 sampling_time);

public:
    QTrend(QString tag_name="",float min_value=0,float max_value=0,QString axis_label="");
    ~QTrend();
    void newSample(qint64 timestamp,float value);

    int getValue(float &value);
    int setValue(QVariant sample_time,QVariant tag_value);

    inline QString getTagName(){return _tag_name;}
    inline float getTimeBase(){return _time_base;}
    float setTimeBase(float time_base);
    void setTrendingSpan(qint64 trendingSpan, qint64 trendingTimeScale=1000,qint64 last_sample_timestamp=0);

    inline double* getRealTimeVectorX(){return _realTimeVectorX;}
    inline double* getRealTimeVectorY(){return _realTimeVectorY;}
    inline int getRealTimeNSamples(){return SPAN_SAMPLE_SIZE;}
    inline double getRealTimeSpan(){return _realTimeSpan;}
    inline double* getTrendingPenX(){return _trendingPenX;}
    inline double* getTrendingPenY(){return _trendingPenY;}
    inline int getTrendingPenSamples(){return SPAN_SAMPLE_SIZE;}
    inline qint64 getTrendingSpan(){return _trendingSpan;}
    inline qint64 getTrendingTimeScale(){return _trendingTimeScale;}
    inline double getIniSampleScaledTime(){return _trendingPenX[0];}
    inline double getLastSampleScaledTime(){return _trendingPenX[SPAN_SAMPLE_SIZE-1];}
    inline qint64 getTrendingMinX(){return _value.at(0);}
    inline qint64 getTrendingMaxX(){return _value.last();}
    inline float getMinValue(){return _min_value;}
    inline float getMaxValue(){return _max_value;}
    inline QString getAxisLabel(){return _axis_label;}

};

#endif // QTREND_H
