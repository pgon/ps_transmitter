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


#ifndef QHMITRENDING_H
#define QHMITRENDING_H

#include <QRadioButton>
#include <QGroupBox>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include "QTrend.h"
#include "qwt_compat.h"
#include "qwt_plot_picker.h"
#include "qwt_plot_marker.h"
#include "qwt_series_data.h"
typedef QwtSeriesData<double> QwtData;

class QHMIPlotPicker : public QwtPlotPicker
{
protected:

    QwtText trackerText(const QPoint& pos) const
    {
        QString info;
        float time = this->plot()->invTransform(QwtPlot::xBottom, pos.x());
        info.sprintf("%5.1f",time);
        info = this->plot()->axisTitle(QwtPlot::xBottom).text() + "=" + info;
        for( int i=0;i<this->plot()->itemList().count();i++ )
        {
            QwtPlotItem::RttiValues item_type = (QwtPlotItem::RttiValues) this->plot()->itemList().at(i)->rtti();
            if( item_type!=QwtPlotItem::Rtti_PlotCurve )
                continue;
            QwtPlotCurve *c=(QwtPlotCurve *) this->plot()->itemList().at(i);
            if (!c->isVisible() )
                continue;

            int j;
/*            for( j=0;j<c->dataSize();j++ )
                if( c->x(j)>time )
                    break;*/
            if( j>=c->dataSize() )
                info="";
            /*else
                info = info + QString("\n") + c->title().text() + QString(" = ") + QString::number(c->y(j),'f',1);
                */
        };

        return QwtText(info);
    }

public:
    QHMIPlotPicker(int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode,QwtPlotCanvas *canvas)
        :QwtPlotPicker(xAxis,yAxis,rubberBand,trackerMode,canvas) {;}

};


class QHMITrending : public QwtPlot
{
    Q_OBJECT

private slots:
    void showCurve(QwtPlotItem *item, bool on);
    void onRectSelection(QwtDoubleRect rect_selection);

private:
    int _trendingType;
    QList<QTrend*>  _trend;
    QList<QwtPlotCurve*> _trendingPenList;
    QList<QwtPlotMarker*> _markerList;

    QHMIPlotPicker *d_picker;

    qint64 _baseTimeSpan;
    QString timeAxisLabel;
    QString timeAxisUnit;

    void setTrendingType(int trendingType);

public:
    enum
    {
        realtimeTrending,
        variableTimeTrending
    };


    QHMITrending(int trendingType=realtimeTrending,QWidget * parent=NULL);
    ~QHMITrending();    

    void setTimeAxisLabel(const QString &label){timeAxisLabel=label;}
    void setTimeAxisUnits(const QString &unit){timeAxisUnit=unit;}
    void applyAxisTitle();

    int addMarker(QwtText penTitle,QColor penColor,int yAxis,QwtPlotMarker::LineStyle line_style,int alignment);
    void setMarkerYValue(int idx_marker,float value);

    void addTrendingPen(QTrend *trend,QString penTitle,QColor penColor=Qt::black, int yAxis=QwtPlot::yRight, bool flagFixScale=false,bool flagFixGrid=false,Qt::PenStyle penStyle=Qt::SolidLine);
    void setAxisYScale(double min, double max, double step = 0);
    void setTimeWindow(qint64 timeSpan=0, qint64 timeScale=1000, qint64 t_last_timestamp=0, QString unitStr="seg");
    void setFixedTimeWindow(double t_ini,double t_last );
    QwtPlotCurve* getTrendingPen(int idx_pen=0)
    {
        if( idx_pen<_trendingPenList.size() )
            return _trendingPenList.at(idx_pen);
        return NULL;
    }
};


class QHMIHisto : public QwtPlot
{
    Q_OBJECT
public:
    explicit QHMIHisto(int nBins,QWidget *parent = 0);
    void setFont(QFont);

signals:

private:
    QwtPlotCurve *curve_histo ;
    QVector<double> histo_data_x;
    QVector<double> histo_data_y;

    QwtPlotCurve *curve_fit;
    QVector<double> fit_data_x;
    QVector<double> fit_data_y;

    QwtData *_curve_data;

    int _nBins;

    void calcHistogram(void);

public slots:
    void mapData( QwtData *curve_data ){_curve_data=curve_data;}
    void replot( double mean,double stddev );
};


class QHMITrendingVar : public QFrame
{
    Q_OBJECT

private slots:
    void onTimeChanged(void);
    void onContextMenuRequest(QPoint p);

private:
    QHMITrending *_trending;
    QRadioButton *radio[5];
    qint64 timeSpanVector[5];
    qint64 timeScaleVector[5];
    QString axisTimeUnits[5];
    QGroupBox *gbTimeSelector;
    QFrame *frSelector;

public:
    inline QHMITrending* trending(){return _trending;}

    QHMITrendingVar(int trendingType,QWidget * parent=NULL,bool flag_docked=true);
    ~QHMITrendingVar();    

    int setWindowTime(int windowTime=0);
    void setFont(QFont);
};


#endif // QHMITRENDING_H
