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

#include "trendingThread.h"
#include "qwt_plot_grid.h"
#include "qwt_legend.h"
#include "qwt_symbol.h"


trendingThread::trendingThread(QwtPlot *plotter, QObject *parent) :
    QThread(parent)
{
    n_samples=0;
    autonomia_promedio=0;


    plotter_ = plotter;
    plotter_->setTitle( "Data Trending" );
    plotter_->setCanvasBackground( Qt::black );
    plotter_->setAxisScale( QwtPlot::xBottom, -2000, 0);
    plotter_->setAxisScale( QwtPlot::yLeft, -100, 100);
    //plot->insertLegend( new QwtLegend() );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setYAxis(QwtPlot::yLeft);
    grid->setXAxis(QwtPlot::xBottom);
    grid->enableXMin(true);
    grid->setMajorPen(QPen(QColor(128,128,128,100), 0, Qt::DotLine));
    grid->setMinorPen(QPen(QColor(50,50,50,100), 0 , Qt::DotLine));
    grid->attach( plotter_ );

    curve = new QwtPlotCurve();
    curve->setTitle( "Measurement 1" );
    //curve->setPen( Qt::red, 1 ),
    curve->setPen( Qt::yellow, 1 ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve->setYAxis(QwtPlot::yLeft);

    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::Ellipse,QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 4, 4 ) );
    curve->setSymbol( symbol );

    // set up the initial plot data
    for( int index=0; index<100; ++index )
    {
        xData[index] = index;
        yData[index] = 0;
    }
    plotDataSize=0;
    //curve->setSamples(xData, yData, plotDataSize);
    curve->attach( plotter_ );

    plotter_->replot();
    plotter_->show();

    refresh_timer = new QTimer();
    connect(refresh_timer, SIGNAL(timeout()), this, SLOT(refreshPlotter()));
    last_refresh = QDateTime::currentDateTime();
    refresh_timer->start(2000);
}

/*void trendingThread::run()
{
    exec();
}
*/

void trendingThread::refreshPlotter()
{
    QMutexLocker locker(&mutex);
    QDateTime time_stamp = QDateTime::currentDateTime();
    double delta_t = last_refresh.msecsTo(time_stamp)/1000.0;
    for( int i=0;i<plotDataSize;i++ )
        xData[i]-=delta_t;
    last_refresh = time_stamp;
    curve->setSamples(xData, yData, plotDataSize);
    plotter_->replot();
}

void trendingThread::appendSample(int socket_src, double sample_value, QDateTime sample_time)
{
    QMutexLocker locker(&mutex);
    QDateTime time_stamp = QDateTime::currentDateTime();

    if( ++plotDataSize>N_SAMPLES )
    {
        memmove( yData, yData+1, (N_SAMPLES-1) * sizeof(double) );
        memmove( xData, xData+1, (N_SAMPLES-1) * sizeof(double) );
        plotDataSize=100;
    }

    /*
    float corriente = (sample_value*1.4 + (20000-sample_value)*71.2)/20000.0/1.25;
    float autonomia = 1500/corriente*0.7;
    n_samples++;
    autonomia_promedio = (autonomia+autonomia_promedio*(n_samples-1))/n_samples;

    yData[plotDataSize-1] = corriente;
    */
    //TODO: En realidad pordria restarle el offset de la muestra en el tiempo. No iria en 0
    yData[plotDataSize-1] = sample_value;
    xData[plotDataSize-1] = time_stamp.msecsTo(sample_time)/1000.0;
    curve->setSamples(xData, yData, plotDataSize);
    plotter_->replot();
}
