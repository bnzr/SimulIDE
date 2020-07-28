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
#include "customplotwidget.h"

class Timeplot;

class MAINMODULE_EXPORT TimeplotWidget : public QWidget, public eElement
{
    Q_OBJECT

    public:
        TimeplotWidget( QWidget *parent , int timeplotWidgetWidth,  int timeplotWidgetHeight);
        ~TimeplotWidget();
        
        void setTimeplot( Timeplot* timeplot );
        void read();
        void clear();
        void setupWidget();
	void set_time_plot_window_flag (bool t);
        
        virtual void simuClockStep();
        virtual void resetState();

    public slots:
	void handlePlotButton();
        void handleScaleButton();
	void widgetDestroyed(QObject*);
	void time_axis_value(int);
	void resetTimeplotWindow();
	
    private:	
        QCustomPlot* customPlot;
	CustomPlotWidget *customPlotWidget;
	
        Timeplot* m_timeplot;
	QPushButton *m_button_plot;
	QPushButton *m_button_reset;

        uint64_t m_step;
	double m_start_time;
	double m_step_time;
 	double m_step_time_last;
        double m_plot_clock = 0.0001;
        bool m_time_plot_window = false;
        int m_cnt_replot = 0;
        int m_cnt_replot_ok = 1000;

	QSlider *m_time_axis_slider;
	int m_time_axis_window = 4;
	double m_time_axis_window_sec = 5.0;
	double v_time_axis_window[7] = {0.1, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0};
	int m_time_axis_window_min = 0;
	int m_time_axis_window_max = 6;
	QLabel *m_time_axis_label;

	bool m_reset_graph = false;
};

#endif
