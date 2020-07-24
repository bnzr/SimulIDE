/***************************************************************************
 *   Copyright (C) 2016 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#include "customplotwidget.h"
 
#include <QBrush>
#include <QPen>

CustomPlotWidget::CustomPlotWidget(int width, int height)
  : QWidget()
{
  m_width = width;
  m_height = height;
  gridLayout = new QHBoxLayout;
  
  setObjectName(QStringLiteral("customPlot"));
  resize (width, height);
  setWindowFlags(Qt::WindowStaysOnTopHint);
  //setAttribute( Qt::WA_DeleteOnClose );
  //connect(customPlotWidget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)) );

    customPlot = new QCustomPlot( this );
    customPlot->setObjectName(QStringLiteral("customPlot"));
    customPlot->resize (800,400);
 
    customPlot->addGraph(); // red line
    customPlot->graph(0)->setPen(QPen(Qt::red));
    customPlot->addGraph(); // blue line
    customPlot->graph(1)->setPen(QPen(Qt::blue));
    customPlot->addGraph(); // gree line
    customPlot->graph(2)->setPen(QPen(Qt::green));
    customPlot->addGraph(); // cyan line
    customPlot->graph(3)->setPen(QPen(Qt::cyan));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%s.%z");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-6, 6);

    gridLayout->addWidget(customPlot);
    setLayout(gridLayout);
    
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    mouseMarkerX = new QCPItemStraightLine(customPlot);
    mouseMarkerY = new QCPItemStraightLine(customPlot);
    mouseMarkerX->point1->setCoords(0, 0);
    mouseMarkerX->point2->setCoords(0, 1);
    mouseMarkerY->point1->setCoords(0, 0);
    mouseMarkerY->point2->setCoords(1, 0);

   connect(customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(onMouseMove(QMouseEvent *)));

   
}

QSize CustomPlotWidget::minimumSizeHint() const  {  return QSize( m_width, m_height );  }

QSize CustomPlotWidget::sizeHint() const  { return QSize( m_width, m_height ); }

void CustomPlotWidget::paintEvent( QPaintEvent* event )
{
    QPainter p( this );
    //qDebug() << "CustomPlotWidget::paintEvent";
}

void CustomPlotWidget::onMouseMove(QMouseEvent * event)
{
  double x = customPlot->xAxis->pixelToCoord(event->pos().x());
  double y = customPlot->xAxis->pixelToCoord(event->pos().y());
  qDebug() << "mouse" << event->x() << event->y() << x << y;
  double positionX = customPlot->xAxis->pixelToCoord( event->pos().x() );
  double positionY = customPlot->yAxis->pixelToCoord( event->pos().y() );
  crosshairpen.setColor(Qt::darkYellow);
  mouseMarkerX->setPen(crosshairpen);
  mouseMarkerY->setPen(crosshairpen);
  mouseMarkerX->point1->setCoords(positionX, 0);
  mouseMarkerX->point2->setCoords(positionX, 1);
  mouseMarkerY->point1->setCoords(0, positionY);
  mouseMarkerY->point2->setCoords(1, positionY);
  customPlot->replot();
}


 
QCustomPlot* CustomPlotWidget::getCustomPlot()
{
  return customPlot;
}

void CustomPlotWidget::setTimeplotWidget(QWidget * parent)
{
  timeplot = parent;
}

void CustomPlotWidget::closeEvent(QCloseEvent * event)
{
  qDebug() << "about to close";
  //close();
  //parent->set_time_plot_window_flag (false);
  emit customPlotWidgetAboutToClose();
  //m_time_plot_window = false;
  event->accept();
}



#include "moc_customplotwidget.cpp"
