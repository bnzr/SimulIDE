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
}

void TimeplotWidget::resetState()
{
    clear();
}

void TimeplotWidget::simuClockStep()
{
  m_step = Simulator::self()->step();
  m_step_time = (double) m_step * 1e-6;
  if (((m_step_time - m_step_time_last) > m_plot_clock) && m_time_plot_window)
    {
      qDebug() << "TimeplotWidget::simuClockStep"<< m_step << m_step_time;
      for (int i_chan=1; i_chan<=2; i_chan++)
	{      
	  double v = 0.0;
	  //if (m_timeplot->channelOn(i_chan))
	    v = m_timeplot->getVolt(i_chan);
	  customPlot->graph(i_chan-1)->addData(m_step_time, v);
	  customPlot->xAxis->setRange(m_step_time, 4, Qt::AlignRight);
	}
      m_cnt_replot ++;
      if (m_cnt_replot == m_cnt_replot_ok)
	{
	  customPlot->replot();
	  m_cnt_replot = 0;
	}

      /*
      if (m_plot_fill[m_wr] < 0) m_plot_fill[m_wr] = 0;
      if (m_plot_fill[m_wr] < m_plot_maxpt)
	{
	  m_plot_time[m_wr][m_plot_fill[m_wr]] = m_step_time;
	  for (int i_chan=1; i_chan<=4; i_chan++)
	    {
	      double v = 0.0;
	      if (m_timeplot->channelOn(i_chan))
		v = m_timeplot->getVolt(i_chan);
	      m_plot_signal[m_wr][i_chan-1][m_plot_fill[m_wr]] = v;
	    }
	  m_plot_fill[m_wr] ++;
	}
      m_step_time_last = m_step_time;
      */
      m_step_time_last = m_step_time;
    }
}

void TimeplotWidget::read()
{
  //for (int i_chan=1; i_chan<=4; i_chan++) qDebug() << i_chan << m_timeplot->channelOn(i_chan);
  m_step = Simulator::self()->step();
  m_step_time = (double) m_step * 1e-6;
  //qDebug() << "TimeplotWidget::read" << m_plot_fill << m_step << m_step_time;
  /*
  if (m_plot_fill < 0) m_plot_fill = 0;
  if (m_plot_fill < m_plot_maxpt)
    {
      m_plot_time[m_plot_fill] = m_step_time;
      for (int i_chan=1; i_chan<=4; i_chan++)
	{
	  double v = 0.0;
	  if (m_timeplot->channelOn(i_chan))
  	      v = m_timeplot->getVolt(i_chan);
	  m_plot_signal[i_chan-1][m_plot_fill] = v;
	}
      m_plot_fill ++;
    }
  */
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
     m_button_reset = new QPushButton ("Reset", this);
     m_button_reset->setStyleSheet("QPushButton {font-size: 12px;font-family: Arial;background-color : lightblue; color : black;}");
     m_button_reset->setGeometry(QRect(QPoint(10, 30), QSize(50, 20)));
     connect(m_button_reset, SIGNAL (released()), this, SLOT (handleResetButton()));
 
    
    //m_display = new RenderTimeplot( size, size, this );
    //m_display->setObjectName( "timeplot" );
    
              
    //connect( m_autoCheck, SIGNAL( stateChanged(int) ),
    //         this,        SLOT  ( autoTimeplotChanged(int)) );
}

void TimeplotWidget::realtimeDataSlotDemo()
{
  static QTime time(QTime::currentTime());
  // calculate two new data points:
  double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
  static double lastPointKey = 0;
  if (key-lastPointKey > 0.002) // at most add point every 2 ms
  {
    // add data to lines:
    customPlot->graph(0)->addData(key, qSin(key)+qrand()/(double)RAND_MAX*1*qSin(key/0.3843));
    customPlot->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
    // rescale value (vertical) axis to fit the current data:
    //ui->customPlot->graph(0)->rescaleValueAxis();
    //ui->customPlot->graph(1)->rescaleValueAxis(true);
    lastPointKey = key;
  }
  // make key axis range scroll with the data (at a constant range size of 8):
  customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
  customPlot->replot();
  
  // calculate frames per second:
  static double lastFpsKey;
  static int frameCount;
  ++frameCount;
  if (key-lastFpsKey > 2) // average fps over 2 seconds
  {
    /*ui->statusBar->showMessage(
          QString("%1 FPS, Total Data points: %2")
          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(ui->customPlot->graph(0)->data()->size()+ui->customPlot->graph(1)->data()->size())
          , 0);
    */
    qDebug() << QString("%1 FPS, Total Data points: %2")
      .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
      .arg(customPlot->graph(0)->data()->size()+customPlot->graph(1)->data()->size());
    lastFpsKey = key;
    frameCount = 0;
  }
}

void TimeplotWidget::realtimeDataSlot()
{
  qDebug() << "TimeplotWidget::realtimeDataSlot" << m_plot_fill[m_rd];
  int cnt = 0;
  double key;
  if (m_plot_fill[m_rd] >= 0)
    {
      for (int ip=0; ip<=m_plot_fill[m_rd]; ip++) 
	{
	  key = m_plot_time[m_rd][ip];
	  for (int i_chan=1; i_chan<=2; i_chan++)
	    {      
	      customPlot->graph(i_chan-1)->addData(key, m_plot_signal[m_rd][i_chan-1][ip]);
	      customPlot->xAxis->setRange(key, 4, Qt::AlignRight);
	    }
	  cnt ++;
	}
    }
  customPlot->replot();
  m_plot_fill[m_rd] = -1;
  if (m_rd == 0)
    {
      m_rd = 1;
      m_wr = 0;
    }
  else
    {
      m_rd = 0;
      m_wr = 1;
    }
      
}

void TimeplotWidget::handlePlotButton()
{
    // change the text
    //m_button->setText("Plot Active");
    // resize button
    //m_button->resize(100,100);

    QWidget *wdg = new QWidget;
    wdg->resize (800,400);
    wdg->setWindowFlags(Qt::WindowStaysOnTopHint);

    customPlot = new QCustomPlot( wdg );
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
  
    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    //dataTimer.start(10); // Interval 0 means to refresh as fast as possible
    wdg->show();
    m_time_plot_window = true;

}
void TimeplotWidget::handleResetButton()
{
}

#include "moc_timeplotwidget.cpp"
