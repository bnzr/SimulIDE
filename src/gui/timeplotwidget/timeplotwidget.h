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

#ifndef TIMEPLOTWIDGET_H
#define TIMEPLOTWIDGET_H

#include <QtWidgets>

#include "e-element.h"
#include "probe.h"

#include <qcustomplot.h>

class Timeplot;

class MAINMODULE_EXPORT TimeplotWidget : public QWidget, public eElement
{
    Q_OBJECT

    public:
        TimeplotWidget( QWidget *parent );
        ~TimeplotWidget();
        
        void setTimeplot( Timeplot* timeplot );
        void read();
        void clear();
        void setupWidget();
        
        virtual void simuClockStep();
        virtual void resetState();
        //void simuClockStep();
        //void resetState();
	void realtimeDataSlotDemo();

    public slots:
	void realtimeDataSlot();
	void handlePlotButton();
        void handleResetButton();

    private:
        QCustomPlot* customPlot;
	QTimer dataTimer;
        
        Timeplot* m_timeplot;
	QPushButton *m_button_plot;
	QPushButton *m_button_reset;

        uint64_t m_step;
	double m_step_time;
 	double m_step_time_last;
        double m_plot_clock = 0.01;
        double m_plot_signal[2][4][200];
  	double m_plot_time[2][200];
        int m_plot_fill[2]= {0, 0};
        int m_plot_maxpt = 200;
        int m_wr = 0;
        int m_rd = 1;
        bool m_time_plot_window = false;
        int m_cnt_replot = 0;
        int m_cnt_replot_ok = 10;
};

#endif
