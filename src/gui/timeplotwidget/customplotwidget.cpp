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
#include "simulator.h"

#include <QBrush>
#include <QPen>
#include <QThread>

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
    QPen* pen_red = new QPen(Qt::red);
    int pen_width = pen_red->width()*4;
    pen_red->setWidth(pen_width);
    customPlot->graph(0)->setPen(*pen_red);
    
    customPlot->addGraph(); // blue line
    QPen* pen_blue = new QPen(Qt::blue);
    pen_blue->setWidth(pen_width);
    customPlot->graph(1)->setPen(*pen_blue);
    
    customPlot->addGraph(); // gree line
    QPen* pen_green = new QPen(Qt::green);
    pen_green->setWidth(pen_width);
    customPlot->graph(2)->setPen(*pen_green);
    
    customPlot->addGraph(); // cyan line
    QPen* pen_cyan = new QPen(Qt::cyan);
    pen_cyan->setWidth(pen_width);
    customPlot->graph(3)->setPen(*pen_cyan);

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

    textItem = new QCPItemText(customPlot);
    textItem->setText("");
    textItem->position->setCoords(QPointF(0, 0.5));
    textItem->setFont(QFont(font().family(), 16));

    connect(customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(onMouseMove(QMouseEvent *)));

    // add a timer to prevent data scrolling when window is resized
    m_resizing = false;
    timer_resizing = new QTimer(this);
    connect(timer_resizing, SIGNAL(timeout()), this, SLOT(timeoutResizing()));

}

QSize CustomPlotWidget::minimumSizeHint() const  {  return QSize( m_width, m_height );  }

QSize CustomPlotWidget::sizeHint() const  { return QSize( m_width, m_height ); }
/*
void CustomPlotWidget::paintEvent( QPaintEvent* event )
{
    QPainter p( this );
    //qDebug() << "CustomPlotWidget::paintEvent";
}
*/
void CustomPlotWidget::resizeEvent( QResizeEvent* event )
{
  if (customPlot->isReplotting())
    {
      QThread::msleep(100);
    }
  if (Simulator::self()->isRunning())
    {
      customPlot->setVisible(false);
      
      if (!m_resizing)
	{
	  m_resizing = true;
	  //qDebug() << "CustomPlotWidget::resizeEvent"<< m_resizing;
	  timer_resizing->setSingleShot(true);
	  timer_resizing->start(50);
	}
      else
	{
	  //qDebug() << "CustomPlotWidget::resizeEvent"<< timer_resizing->remainingTime() << "remaining";
	  timer_resizing->stop();
	  timer_resizing->setSingleShot(true);
	  timer_resizing->start(50);
	}
    }
    
}
  

void CustomPlotWidget::replot()
{
  //qDebug() << "CustomPlotWidget::replot" << m_resizing << resizing();
  if (!resizing())
    {
      //qDebug() << "CustomPlotWidget::replot" << m_resizing << resizing();
      if (customPlot->isVisible()) customPlot->replot();
    }
  //else  qDebug() << "CustomPlotWidget::replot"<< "resizing lock refresh";
}


bool CustomPlotWidget::resizing()
{
  return m_resizing;
}

void CustomPlotWidget::timeoutResizing ()
{
  //qDebug() << "CustomPlotWidget::timeoutResizing" << m_resizing;
  //qDebug() << "CustomPlotWidget::timeoutResizing" << m_resizing;
  customPlot->setVisible(true);
  m_resizing = false;
  //timer_resizing->stop();
}

void CustomPlotWidget::onMouseMove(QMouseEvent * event)
{
  //double x = customPlot->xAxis->pixelToCoord(event->pos().x());
  //double y = customPlot->xAxis->pixelToCoord(event->pos().y());
  //qDebug() << "mouse" << event->x() << event->y() << x << y;
  double x = customPlot->xAxis->pixelToCoord( event->pos().x() );
  double y = customPlot->yAxis->pixelToCoord( event->pos().y() );
  //qDebug() << "yaxis" << customPlot->yAxis->range().lower << customPlot->yAxis->range().upper;
  double lower_bound = customPlot->yAxis->range().lower;
  double upper_bound = customPlot->yAxis->range().upper;
  double range = upper_bound-lower_bound;
  crosshairpen.setColor(Qt::darkYellow);
  mouseMarkerX->setPen(crosshairpen);
  mouseMarkerY->setPen(crosshairpen);
  mouseMarkerX->point1->setCoords(x, 0);
  mouseMarkerX->point2->setCoords(x, 1);
  mouseMarkerY->point1->setCoords(0, y);
  mouseMarkerY->point2->setCoords(1, y);
  double offs_y = -0.03*range;
  if (y < lower_bound+0.2*range) offs_y = 0.03*range;
  QString stx = QString::number(x, 'f', 4);
  QString sty = QString::number(y, 'f', 4);
  textItem->setText(QString("x=%1 s , y=%2 V").arg(stx).arg(sty));
  textItem->position->setCoords(QPointF(x, y+offs_y));

  //if (customPlot->isVisible())  customPlot->replot();
  replot();
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
