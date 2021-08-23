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

#include <QVector>
#include "QTrend.h"
#include <QDebug>

QTrend::QTrend(QString tag_name,float min_value,float max_value,QString axis_label)
{
    memset(_realTimeVectorY,0,SPAN_SAMPLE_SIZE*sizeof(*_realTimeVectorY));

    _tag_name = tag_name;
    _min_value = min_value;
    _max_value = max_value;
    _axis_label = axis_label;

    _time.reserve(MAX_SPAN_SIZE);    
    _value.reserve(MAX_SPAN_SIZE);
}

QTrend::~QTrend()
{
    _value.clear();
    _time.clear();
}

float QTrend::setTimeBase(float time_base)
{
    _time_base = time_base;
    if( _time_base<0 || _time_base>1000 )
        _time_base=1000;
    _time_base_factor = 1000/_time_base;
    _n_sample=0;
    if( _time_base_factor==0 )
        _time_base_factor=1;
    _time_base = 1000/_time_base_factor;
    _realTimeSpan = SPAN_SAMPLE_SIZE*_time_base;
    _trendingSpan = 0;

    // Preparo la escala de tiempo realtime
    for( int i=0;i<SPAN_SAMPLE_SIZE;i++ )
        _realTimeVectorX[i]=(-SPAN_SAMPLE_SIZE+i+1)*_time_base/1000;

    return _time_base;
}

// Cargo la curva que se grafica desde un instante final un trendingSpan hacia atrás
void QTrend::setTrendingSpan(qint64 trendingSpan,qint64 trendingTimeScale,qint64 last_sample_timestamp)
{
    QMutexLocker locker(&_trend_mutex);

    if( trendingTimeScale==0 )
        return;
    if( trendingSpan<=0 )
        return;
    if( trendingSpan<_realTimeSpan )        // El span no puede ser menor al del tiempo real
        trendingSpan=_realTimeSpan;

    _trendingSpan = trendingSpan;
    _trendingTimeScale = trendingTimeScale;

    qint64 actual_timestamp=QDateTime::currentMSecsSinceEpoch();
    if( last_sample_timestamp==0 )
        _last_sample_deltaT=0;
    else
        _last_sample_deltaT=last_sample_timestamp-actual_timestamp;
    _last_sample_timestamp=actual_timestamp;


    // Preparo la escala de tiempo del trend
    for( int i=0;i<SPAN_SAMPLE_SIZE;i++ )
        _trendingPenX[i]=_last_sample_deltaT/_trendingTimeScale + ((double)_trendingSpan/(double)(SPAN_SAMPLE_SIZE+1))*(-SPAN_SAMPLE_SIZE+i+1)/_trendingTimeScale;

    // Muestreo el vector
    memset(_trendingPenY,0,SPAN_SAMPLE_SIZE*sizeof(*_trendingPenY));
    int j=_time.count()-1;
    qint64 sampling_time;
    for( int i=SPAN_SAMPLE_SIZE-1;i>=0 && j>=0;i-- )
    {
        sampling_time = actual_timestamp+(_trendingPenX[i]*_trendingTimeScale);
        /*
        while( j>=0 && _time.at(j)> sampling_time )
            j--;

        if( j<0 )
            break;

        qint64 resto1=0,resto2;
        if( j<_time.count()-1 )
        {
            resto1=_time.at(j+1)-sampling_time;
            resto2=_time.at(j)-sampling_time;
            _trendingPenY[i]=(_value.at(j)*resto1+_value.at(j+1)*(-resto2))/(double)(resto1-resto2);
        }
        else
            _trendingPenY[i]=_value.at(j);
            */
        _trendingPenY[i] = findSampleValue(j,sampling_time);
    }
}

double QTrend::findSampleValue(int &idxStart,qint64 sampling_time)
{
    double sampling_value;

    while( idxStart>=0 && _time.at(idxStart)>sampling_time )
        idxStart--;
    if( idxStart<0 )
        return 0;

    qint64 resto1=0,resto2;
    if( idxStart<_time.count()-1 )
    {
        resto1=_time.at(idxStart+1)-sampling_time;
        resto2=_time.at(idxStart)-sampling_time;
        sampling_value=(_value.at(idxStart)*resto1+_value.at(idxStart+1)*(-resto2))/(double)(resto1-resto2);
    }
    else
        sampling_value=_value.at(idxStart);
    return sampling_value;
}


void QTrend::newSample(qint64 timestamp,float value)
{
    QMutexLocker locker(&_trend_mutex);

    // incorporo la muestra en el trend de tiempo real
    memmove(_realTimeVectorY,_realTimeVectorY+1,(SPAN_SAMPLE_SIZE-1)*sizeof(*_realTimeVectorY));
    _realTimeVectorY[SPAN_SAMPLE_SIZE-1]=value;

    // incorporo la muestra en el vector
    if( ++_n_sample>=_time_base_factor )
    {
        _n_sample=0;
        if( _value.size()==_value.capacity() )
        {
            _value.pop_front();
            _time.pop_front();
        }
        _value.push_back(value);
        _time.push_back(timestamp);
    }

    // actualizo el trendingPen para la ventana de tiempo solicitada
    if( _trendingSpan>0 )
    {
        // Si ya paso un periodo desde la ultima muestra ingresada => tomo una nueva
        if( timestamp-_last_sample_timestamp>=((double)_trendingSpan/(double)SPAN_SAMPLE_SIZE) )
        {
            memmove(_trendingPenY,_trendingPenY+1,(SPAN_SAMPLE_SIZE-1)*sizeof(*_trendingPenY));
            if( _last_sample_deltaT==0 )
                _trendingPenY[SPAN_SAMPLE_SIZE-1]=value;
            else
            {
                qint64 sampling_time = timestamp+_last_sample_deltaT;
                int j=_time.count()-1;
                _trendingPenY[SPAN_SAMPLE_SIZE-1] = findSampleValue(j,sampling_time);
            }

            _last_sample_timestamp=timestamp;
        }
    }
}

