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

TimeplotWidget::TimeplotWidget(  QWidget *parent, int  timeplotWidgetWidth, int timeplotWidgetHeight )
            : QWidget( parent )
            , eElement( "timeplot" )  // "oscope"
{
  qDebug() << "TimeplotWidget::TimeplotWidget get types" << typeid(*parent).name() << typeid(*this).name() << timeplotWidgetWidth << timeplotWidgetHeight; 
    setMaximumSize(QSize(timeplotWidgetWidth, timeplotWidgetHeight));
    setMinimumSize(QSize(timeplotWidgetWidth, timeplotWidgetHeight));
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
      //qDebug() << "TimeplotWidget::simuClockStep"<< m_step << m_step_time;
      for (int i_chan=1; i_chan<=4; i_chan++)
	{      
	  double v = m_timeplot->getVolt(i_chan);
	  customPlot->graph(i_chan-1)->addData(m_step_time, v);
	}
      customPlot->xAxis->setRange(m_step_time,  m_time_axis_window_sec, Qt::AlignRight);
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
     m_button_plot->setGeometry(QRect(QPoint(5, 5), QSize(50, 20)));
     connect(m_button_plot, SIGNAL (released()), this, SLOT (handlePlotButton()));
     
     m_button_reset = new QPushButton ("Scale Y", this);
     m_button_reset->setStyleSheet("QPushButton {font-size: 12px;font-family: Arial;background-color : lightblue; color : black;}");
     m_button_reset->setGeometry(QRect(QPoint(5, 30), QSize(50, 20)));
     
     connect(m_button_reset, SIGNAL (released()), this, SLOT (handleScaleButton()));
     m_time_axis_slider = new QSlider(Qt::Horizontal, this);
     //m_time_axis_slider->setGeometry(QRect(QPoint(5, 55), QSize(110, 5)));
     //m_time_axis_slider->resize (80,10);
     m_time_axis_slider->move(60,55);
     m_time_axis_slider->setTickPosition (QSlider::TicksBothSides);
     m_time_axis_slider->setMinimum(m_time_axis_window_min);
     m_time_axis_slider->setMaximum(m_time_axis_window_max);
     m_time_axis_slider->setSingleStep(1);
     m_time_axis_slider->setValue(m_time_axis_window);
     
     m_time_axis_window_sec =  v_time_axis_window[m_time_axis_window];
     QString label_text = QString("%1 s").arg(m_time_axis_window_sec,8,'g');
     m_time_axis_label = new QLabel (label_text, this);
     m_time_axis_label->setStyleSheet("QLabel {font-size: 12px;font-family: Arial;background-color : lightblue; color : black;}");
     m_time_axis_label->setGeometry(QRect(QPoint(5, 55), QSize(50, 20)));
     
     connect(m_time_axis_slider, SIGNAL(valueChanged(int)), this, SLOT(time_axis_value(int)));
     
}

void TimeplotWidget::handlePlotButton()
{
  if (!m_time_plot_window)
    {
      customPlotWidget = new CustomPlotWidget (800, 400);
      customPlot = customPlotWidget->getCustomPlot();
      customPlotWidget->setTimeplotWidget(this);
      uint64_t c_step = Simulator::self()->step();
      m_start_time = (double) c_step * 1e-6;
      m_step_time_last = m_start_time;

      m_time_plot_window = true;
      connect( customPlotWidget,
	       SIGNAL(customPlotWidgetAboutToClose()),
	       SLOT(resetTimeplotWindow()));
      customPlotWidget->show();
    }
}

void TimeplotWidget::resetTimeplotWindow()
{
  m_time_plot_window = false;
  qDebug() << "TimeplotWidget::resetTimeplotWindow" << m_time_plot_window;
}

void TimeplotWidget::handleScaleButton()
{
  qDebug() << "rescale y axis";
  customPlot->yAxis->rescale();
  // add an upper and lower margin
  double lower_bound = customPlot->yAxis->range().lower;
  double upper_bound = customPlot->yAxis->range().upper;
  double range = upper_bound-lower_bound;
  customPlot->yAxis->setRange(lower_bound-range*0.02,upper_bound+range*0.02);
  customPlot->replot();
}

void TimeplotWidget::widgetDestroyed(QObject*)
{
  m_time_plot_window = false;
}  

void TimeplotWidget::time_axis_value(int val)
{
  qDebug() << "time_axis_value"<< val << m_time_axis_slider->tickPosition();
  m_time_axis_window = val;
  m_time_axis_window_sec =  v_time_axis_window[m_time_axis_window];
  QString label_text = QString("%1 s").arg(m_time_axis_window_sec,8,'g');
  m_time_axis_label->setText(label_text);
  if (m_time_plot_window)
    {
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
}
void TimeplotWidget::set_time_plot_window_flag (bool t)
{
  m_time_plot_window = t;
}
									      
#include "moc_timeplotwidget.cpp"
