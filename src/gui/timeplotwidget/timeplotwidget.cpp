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

#include "timeplotwidget.h"
#include "simulator.h"
#include "timeplot.h"

#include <math.h>   // fabs(x,y)

TimeplotWidget::TimeplotWidget(  QWidget *parent  )
            : QWidget( parent )
            , eElement( "timeplot" )  // "oscope"
{
    qDebug() << "TimeplotWidget::TimeplotWidget get types" << typeid(*parent).name() << typeid(*this).name() ; 
    setMaximumSize(QSize(60, 100));
    setMinimumSize(QSize(60, 100));
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::lightGray);
    setAutoFillBackground(true);
    setPalette(pal);
    qDebug() << "position" << pos(); 
    this->setVisible( true );
    m_step = Simulator::self()->step();
    m_step_time_last = (double) m_step * 1e-6;

}
TimeplotWidget::~TimeplotWidget()
{
  qDebug() << "End of TimeplotWidget";
  if (m_time_plot_window)
    {
      customPlotWidget->close();
      m_time_plot_window = false;
      qDebug() << "End of CustomPlotlotWidget";
    }
}

void TimeplotWidget::resetState()
{
    clear();
}

void TimeplotWidget::read()
{
}

void TimeplotWidget::simuClockStep()
{
  m_step = Simulator::self()->step();
  m_step_time = (double) m_step * 1e-6;
  if (m_time_plot_window)
    {
  if (((m_step_time - m_step_time_last) > m_plot_clock) && m_time_plot_window)
    {
      qDebug() << "TimeplotWidget::simuClockStep"<< m_step << m_step_time;
      for (int i_chan=1; i_chan<=2; i_chan++)
	{      
	  double v = 0.0;
	  //if (m_timeplot->channelOn(i_chan))
	    v = m_timeplot->getVolt(i_chan);
	  customPlot->graph(i_chan-1)->addData(m_step_time, v);
	  customPlot->xAxis->setRange(m_step_time,  m_time_axis_window, Qt::AlignRight);
	}
      m_cnt_replot ++;
      if (m_cnt_replot == m_cnt_replot_ok)
	{
	  customPlot->replot();
	  m_cnt_replot = 0;
	}
      m_step_time_last = m_step_time;
    }
    }
}



void TimeplotWidget::clear()
{
  // reset plot here
}

void TimeplotWidget::setTimeplot( Timeplot* timeplot )
{
    m_timeplot = timeplot;
    
    if( timeplot  ) Simulator::self()->addToSimuClockList( this );
    else          Simulator::self()->remFromSimuClockList( this );
}


void TimeplotWidget::setupWidget()
{
     m_button_plot = new QPushButton ("Plot", this);
     m_button_plot->setStyleSheet("QPushButton {font-size: 12px;font-family: Arial;background-color : lightblue; color : black;}");
     m_button_plot->setGeometry(QRect(QPoint(10, 5), QSize(50, 20)));
     connect(m_button_plot, SIGNAL (released()), this, SLOT (handlePlotButton()));
     m_button_reset = new QPushButton ("Scalet", this);
     m_button_reset->setStyleSheet("QPushButton {font-size: 12px;font-family: Arial;background-color : lightblue; color : black;}");
     m_button_reset->setGeometry(QRect(QPoint(10, 30), QSize(50, 20)));
     connect(m_button_reset, SIGNAL (released()), this, SLOT (handleScaleButton()));
     m_time_axis_slider = new QSlider(Qt::Horizontal, this);
     m_time_axis_slider->setGeometry(QRect(QPoint(10, 55), QSize(50, 20)));
     m_time_axis_slider->setValue(3);
     connect(m_time_axis_slider, SIGNAL(valueChanged(int)), this, SLOT(time_axis_value(int)));
}

void TimeplotWidget::handlePlotButton()
{
  
  if (!m_time_plot_window)
    {
    // change the text
    //m_button->setText("Plot Active");
    // resize button
    //m_button->resize(100,100);

    customPlotWidget = new QWidget;
    customPlotWidget->resize (800,400);
    customPlotWidget->setWindowFlags(Qt::WindowStaysOnTopHint);
    customPlotWidget->setAttribute( Qt::WA_DeleteOnClose );
    connect(customPlotWidget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)) );
 
    customPlot = new QCustomPlot( customPlotWidget );
    customPlot->setObjectName(QStringLiteral("customPlot"));
    customPlot->resize (800,400);
 
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    customPlot->addGraph(); // red line
    customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s.%z");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-6, 6);
    
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
    
    //connect(customPlotWidget,SIGNAL(aboutToQuit()),this,SLOT(closeCustomPlotWidget()));
 
    uint64_t c_step = Simulator::self()->step();
    m_start_time = (double) c_step * 1e-6;
    m_step_time_last = m_start_time;

    m_time_plot_window = true;
    customPlotWidget->show();
    }
}

void TimeplotWidget::closeCustomPlotWidget()
{
  qDebug() << "about to close";
  customPlotWidget->close();
  m_time_plot_window = false;
}  

void TimeplotWidget::handleScaleButton()
{
  customPlotWidget->close();
  m_time_plot_window = false;
}

void TimeplotWidget::widgetDestroyed(QObject*)
{
  m_time_plot_window = false;
}  

void TimeplotWidget::time_axis_value(int val)
{
  qDebug() << "time_axis_value"<< val;
  m_time_axis_window = 1+val;
  if( !Simulator::self()->isRunning() )
    {
      customPlot->xAxis->setRange(0, m_time_axis_window, Qt::AlignRight);
    }
  else
    {
      customPlot->xAxis->setRange(m_step_time, m_time_axis_window, Qt::AlignRight);
    }
  customPlot->replot();
}
#include "moc_timeplotwidget.cpp"
