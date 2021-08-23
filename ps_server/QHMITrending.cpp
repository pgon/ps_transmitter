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


#include "QHMITrending.h"

#include "qwt_plot_scaleitem.h"
#include "qwt_scale_widget.h"
#include "qwt_plot_grid.h"
#include "qwt_legend.h"
#include "qwt_symbol.h"
#include "qwt_legend_label.h"
#include "qwt_legend_data.h"
#include <QDebug>
#include <QLayout>
#include <QLabel>
#include <QDialog>
#include <QPushButton>


QHMITrending::QHMITrending(int trendingType,QWidget *parent):
    QwtPlot(parent)
{
    _trendingType=trendingType;
    _trendingPenList.clear();
    _trend.clear();
    this->setCanvasBackground(Qt::black);

    // legend
    QwtLegend *legend = new QwtLegend;
    //legend->setFrameStyle(QFrame::Box|QFrame::Plain);
    legend->setDefaultItemMode(QwtLegendData::Checkable);

    insertLegend(legend, QwtPlot::TopLegend);
    connect(this, SIGNAL(legendChecked(QwtPlotItem *, bool)),SLOT(showCurve(QwtPlotItem *, bool)));

    this->enableAxis(QwtPlot::yLeft,false);
    this->enableAxis(QwtPlot::yRight,false);



/*    d_picker = new QHMIPlotPicker(QwtPlot::xBottom, QwtPlot::yRight,
        QwtPicker::RectSelection,
        QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn,
        this->canvas());
    d_picker->setRubberBandPen(QPen(Qt::blue, 0, Qt::DotLine));
    d_picker->setTrackerPen(QColor(Qt::white));
    setTrendingType(trendingType);
    connect(d_picker,SIGNAL(selected(QwtDoubleRect)),this,SLOT(onRectSelection(QwtDoubleRect)));*/
}

QHMITrending::~QHMITrending()
{
}

void QHMITrending::setTrendingType(int trendingType)
{
    _trendingType=trendingType;
    if( _trendingType==realtimeTrending )
    {
        d_picker->setSelectionFlags(QwtPlotPicker::NoSelection);
        //disconnect(d_picker,SIGNAL(selected(QwtDoubleRect)),this,SLOT(onRectSelection(QwtDoubleRect)));
    }
    else if( _trendingType==variableTimeTrending )
    {
        d_picker->setSelectionFlags(QwtPlotPicker::RectSelection);
        //connect(d_picker,SIGNAL(selected(QwtDoubleRect)),this,SLOT(onRectSelection(QwtDoubleRect)));
    }
}

int QHMITrending::addMarker(QwtText penTitle,QColor penColor,int yAxis,QwtPlotMarker::LineStyle line_style,int alignment)
{
    QwtPlotMarker *d_mrk1 = new QwtPlotMarker();
    d_mrk1->setValue(0.0, 0.0);
    d_mrk1->setLabel(penTitle);
    d_mrk1->setLineStyle(line_style);
    d_mrk1->setLabelAlignment((Qt::AlignmentFlag) alignment);
    d_mrk1->setLinePen(QPen(penColor, 0, Qt::DashDotLine));
    d_mrk1->setYAxis(yAxis);
    d_mrk1->attach(this);
    _markerList.append(d_mrk1);
    return _markerList.count();
}

void QHMITrending::setMarkerYValue(int idx_marker,float value)
{
    _markerList.at(idx_marker)->setValue(0.0, value);
}

void QHMITrending::addTrendingPen(QTrend *trend,QString penTitle,QColor penColor,int yAxis,bool flagFixScale,bool flagFixGrid,Qt::PenStyle penStyle)
{
    // Preparo el objeto de datos de trending
    QwtPlotCurve* trendPen = new QwtPlotCurve(penTitle);

    QPen penAux;
    penAux.setColor(penColor);
    penAux.setStyle(penStyle);
    trendPen->setPen(penAux);
    trendPen->setRenderHint(QwtPlotItem::RenderAntialiased);
    trendPen->setYAxis(yAxis);


    if( flagFixScale )
    {
        if( trend->getMinValue()==trend->getMaxValue() )
            setAxisAutoScale(yAxis);
        else
            setAxisScale(yAxis,trend->getMinValue(),trend->getMaxValue());
        setAxisTitle(yAxis,trend->getAxisLabel());
        this->enableAxis(yAxis);
    }
    if( flagFixGrid )
    {
        // grid
        QwtPlotGrid *grid = new QwtPlotGrid;
        grid->setYAxis(QwtPlot::yRight);
        grid->setXAxis(QwtPlot::xBottom);
        grid->enableXMin(true);
        grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
        grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
        grid->attach(this);
    }

    trendPen->attach(this);

    _trendingPenList.append(trendPen);
    _trend.append(trend);


    QWidget *w = this->legend()->find(trendPen);
    if ( w && w->inherits("QwtLegendItem") )
    {
        ((QwtLegendItem *)w)->setChecked(true);
    }
}

void QHMITrending::setTimeWindow(qint64 timeSpan,qint64 timeScale,qint64 t_last_timestamp,QString unitStr)
{
    if( timeSpan==0 && t_last_timestamp==0 )
        setTrendingType(realtimeTrending);
    else
        setTrendingType(variableTimeTrending);

    // En caso de no ser producto de un zoom interactivo, almaceno la base, para poder volver atras cuando se haga doble click
    if( t_last_timestamp==0 )
        _baseTimeSpan=timeSpan;
    else if( timeSpan==0 )  // Se hizo un doble click en el visor del trending => span=0 y last_timestamp!=0
    {
        timeSpan=_baseTimeSpan;
        t_last_timestamp=0;
    }

    for( int i=0; i<_trend.count();i++ )
    {
        if( _trendingType==realtimeTrending )
        {
            _trendingPenList.at(i)->setRawData(_trend.at(i)->getRealTimeVectorX(),_trend.at(i)->getRealTimeVectorY(),_trend.at(i)->getRealTimeNSamples());
        }
        else
        {            
            _trend.at(i)->setTrendingSpan(timeSpan,timeScale,t_last_timestamp);
            _trendingPenList.at(i)->setRawData(_trend.at(i)->getTrendingPenX(),_trend.at(i)->getTrendingPenY(),_trend.at(i)->getTrendingPenSamples());
        }
    }
    if( _trendingType==realtimeTrending )
        setAxisScale(QwtPlot::xBottom,-_trend.at(0)->getRealTimeSpan()/1000,0,30);
    else
        setAxisScale(QwtPlot::xBottom,_trend.at(0)->getIniSampleScaledTime(),_trend.at(0)->getLastSampleScaledTime() );
    setTimeAxisUnits(unitStr);
    applyAxisTitle();    
}

void QHMITrending::applyAxisTitle()
{
    QString title;
    if( timeAxisLabel.length()>0 )
    {
        title=timeAxisLabel;
        if( timeAxisUnit.length()>0 )
            title=title+" ["+timeAxisUnit+"]";
        this->setAxisTitle(QwtPlot::xBottom,title);
    }
}

void QHMITrending::setAxisYScale(double min, double max, double step)
{
    setAxisScale(0,min,max,step);
}

void QHMITrending::showCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
}

void QHMITrending::onRectSelection(QwtDoubleRect rect_selection)
{
    double t_ini = rect_selection.left();
    double t_last = rect_selection.right();

    double timeScale=_trend.at(0)->getTrendingTimeScale();
    double timeSpan=(t_last-t_ini)*timeScale;   // El span tiene que estar expresado en ms   

    qint64 t_last_timestamp = QDateTime::currentMSecsSinceEpoch() + t_last*timeScale;


    qDebug() << "Seleccion Tiempo inicial" << t_ini;
    qDebug() << "Seleccion Tiempo final" << t_last;

    setTimeWindow(timeSpan,timeScale,t_last_timestamp,timeAxisUnit);
}


QHMIHisto::QHMIHisto(int nBins,QWidget *parent) :
    QwtPlot(parent)
{
    setCanvasBackground(Qt::black);
    QPen penAux;

    _curve_data=NULL;
    _nBins=nBins;

    curve_histo = new QwtPlotCurve("Histograma");
    penAux.setColor(Qt::red);
    curve_histo->setPen(penAux);
    curve_histo->setBrush(QBrush(QColor(240,210,20,150),Qt::SolidPattern));
    curve_histo->setSymbol(QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::yellow,Qt::SolidPattern),penAux,QSize(5,5)));
    curve_histo->attach(this);

    curve_fit = new QwtPlotCurve("Objetivo");
    penAux.setColor(Qt::green);
    curve_fit->setPen(penAux);
    curve_fit->setBrush(QBrush(QColor(10,210,150,100),Qt::SolidPattern));
    curve_fit->attach(this);

    this->enableAxis(QwtPlot::yLeft,false);
    this->setAxisAutoScale(QwtPlot::yLeft);
    this->setAxisAutoScale(QwtPlot::xBottom);

    histo_data_x.resize(_nBins);
    histo_data_y.resize(_nBins);
    curve_histo->setRawData(histo_data_x.data(),histo_data_y.data(),histo_data_y.size());

    fit_data_x.resize(_nBins);
    fit_data_y.resize(_nBins);
    curve_fit->setRawData(fit_data_x.data(),fit_data_y.data(),fit_data_y.size());

    QHMIPlotPicker *d_picker;
    d_picker = new QHMIPlotPicker(QwtPlot::xBottom, QwtPlot::yRight,
        QwtPicker::PointSelection,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        this->canvas());
    d_picker->setRubberBandPen(QPen(Qt::blue, 0, Qt::DotLine));
    d_picker->setTrackerPen(QColor(Qt::white));
}

void QHMIHisto::setFont(QFont f)
{
    QwtPlot::setFont(f);

    setAxisFont(QwtPlot::xBottom,f);
    f.setBold(true);
    QwtText axisLabel = this->axisTitle(QwtPlot::xBottom);
    axisLabel.setFont(f);
    setAxisTitle(QwtPlot::xBottom,axisLabel);
}

void QHMIHisto::calcHistogram(void)
{
    double szBin;

    if( _curve_data==NULL )
        return;

    if( _nBins<=0 )
        return;

    int vec_size=_curve_data->size();

    // Obtengo los extremos
    double vec_max;
    double vec_min;
    for( int i=0;i<vec_size;i++ )
    {
        if( i==0 || vec_max<_curve_data->y(i) )
            vec_max = _curve_data->y(i);
    }
    for( int i=vec_size-1;i>=0;i-- )
        if( i==vec_size-1 || (vec_min>_curve_data->y(i) && _curve_data->y(i)>0) )
            vec_min = _curve_data->y(i);

    szBin = (float)(vec_max-vec_min) / (float)(_nBins);
    if( szBin==0 )
        return;

    for( int i=0;i<_nBins;i++ )
    {
        histo_data_x[i] = (i+0.5)*szBin+vec_min;
        histo_data_y[i] = 0;
    }

    int idxBin;
    double xBin;
    int n_valid_data=0;
    for( int i=0;i<vec_size;i++ )
    {
        xBin = ((_curve_data->y(i)-vec_min)/szBin)-0.5;
        if( xBin<0 )
            continue;
        idxBin = (int)xBin;
        if( idxBin>=_nBins )
            idxBin=_nBins-1;
        histo_data_y[idxBin]++;
        n_valid_data++;
    }
    for( int i=0;i<_nBins;i++ )
        histo_data_y[i]/=(n_valid_data*szBin);
}


void QHMIHisto::replot( double mean,double stddev )
{
    double x;

    calcHistogram();
    if( fit_data_x.size()<=0 || stddev<=0 )
        return;

    double delta = stddev*6/fit_data_x.size();
    for( int i=0;i<fit_data_x.size();i++ )
    {
        x=(i+0.5)*delta+mean-3*stddev;
        fit_data_x[i]=x;
        fit_data_y[i]=1/(stddev*sqrt(2*M_PI))*exp(-(x-mean)*(x-mean)/2/stddev/stddev);
    }
    //this->setAxisScale(QwtPlot::xBottom,mean-3*stddev,mean+3*stddev,10);
    QwtPlot::replot();
}


QHMITrendingVar::QHMITrendingVar(int trendingType,QWidget * parent,bool flag_docked) : QFrame(parent)
{
    QHBoxLayout *ly=new QHBoxLayout();
    this->setLayout(ly);

    _trending = new QHMITrending(trendingType);

    gbTimeSelector = new QGroupBox(tr("Tiempo"));

    radio[0] = new QRadioButton("RT");
    timeSpanVector[0]=0;
    timeScaleVector[0]=1000;
    axisTimeUnits[0]="seg";
    radio[1] = new QRadioButton("10 min");
    timeSpanVector[1]=10*60*1000;
    timeScaleVector[1]=60*1000;
    axisTimeUnits[1]="min";
    radio[2] = new QRadioButton("30 min");
    timeSpanVector[2]=30*60*1000;
    timeScaleVector[2]=60*1000;
    axisTimeUnits[2]="min";
    radio[3] = new QRadioButton("60 min");
    timeSpanVector[3]=60*60*1000;
    timeScaleVector[3]=60*1000;
    axisTimeUnits[3]="min";
    radio[4] = new QRadioButton("8 hs");
    timeSpanVector[4]=8*60*60*1000;
    timeScaleVector[4]=60*60*1000;
    axisTimeUnits[4]="hs";

    QVBoxLayout *vbox = new QVBoxLayout;
    for( int i=0;i<5;i++ )
    {
        vbox->addWidget(radio[i]);
        connect(radio[i],SIGNAL(toggled(bool)),this,SLOT(onTimeChanged()));
    }
    vbox->addStretch();
    gbTimeSelector->setLayout(vbox);

    ly->addWidget(_trending);
    if( flag_docked )
    {
        ly->addWidget(gbTimeSelector);
    }
    else
    {
        frSelector = new QFrame(this,Qt::Popup);
        frSelector->setFrameShape(QFrame::Box);
        frSelector->setFrameShadow(QFrame::Plain);
        frSelector->setWindowModality(Qt::ApplicationModal);
        frSelector->setWindowTitle("Selector");
        frSelector->setLayout(new QVBoxLayout);
        frSelector->layout()->addWidget(gbTimeSelector);
        QPushButton *btnSalir=new QPushButton("&Ok");
        frSelector->layout()->addWidget(btnSalir);
        connect(btnSalir,SIGNAL(clicked()),frSelector,SLOT(hide()));

        this->setContextMenuPolicy ( Qt::CustomContextMenu );
        connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onContextMenuRequest(QPoint)));
    }

    //radio[0]->setChecked(true);
}

QHMITrendingVar::~QHMITrendingVar()
{

}

void QHMITrendingVar::onTimeChanged()
{
    int i;
    for( i=0;i<5;i++ )
        if( radio[i]->isChecked() )
            break;

    _trending->setTimeWindow(timeSpanVector[i],timeScaleVector[i],0,axisTimeUnits[i]);
}

int QHMITrendingVar::setWindowTime(int windowTime)
{
    if( windowTime<0 || windowTime>=5 )
        return -1;
    radio[windowTime]->setChecked(true);
    return 0;
}

void QHMITrendingVar::setFont(QFont f)
{
    QFrame::setFont(f);
    trending()->setFont(f);
    trending()->setAxisFont(QwtPlot::xBottom,f);
    trending()->setAxisFont(QwtPlot::yRight,f);
    f.setBold(true);
    QwtText axisLabel=trending()->axisTitle(QwtPlot::xBottom);
    axisLabel.setFont(f);
    trending()->setAxisTitle(QwtPlot::xBottom,axisLabel);
    axisLabel=trending()->axisTitle(QwtPlot::yRight);
    axisLabel.setFont(f);
    trending()->setAxisTitle(QwtPlot::yRight,axisLabel);
}

void QHMITrendingVar::onContextMenuRequest(QPoint p)
{
    frSelector->move(mapToGlobal(p));
    frSelector->show();
}
