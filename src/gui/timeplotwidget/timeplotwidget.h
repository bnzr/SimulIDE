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
#include "rendertimeplot.h"
#include "probe.h"

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
        void setupWidget( int size );
        double filter()                 { return m_filter; }
        void setFilter( double filter ) { m_filter = filter; }
        
        virtual void simuClockStep();
        virtual void resetState();
        
    public slots:
        void HscaleTimeplotChanged( int Hscale );
        void VscaleTimeplotChanged( int Vscale );
        void HposTimeplotChanged( int hPos );
        void VposTimeplotChanged( int Vpos );
        void autoTimeplotChanged( int au );

    private:
        QHBoxLayout* m_horizontalLayout;
        QVBoxLayout* m_verticalLayout;
        
        QLabel* m_freqLabel;
        QLabel* m_ampLabel;
        QLabel* m_tickLabel;
        QCheckBox* m_autoCheck;
        QDial* m_HscaleDial;
        QDial* m_VscaleDial;
        QDial* m_HposDial;
        QDial* m_VposDial;
        RenderTimeplot* m_display;
        
        Timeplot* m_timeplot;
        
        int m_data[140];
        int m_counter;
        int m_step;
        int m_totalP;
        int m_ticksPs;
        int m_tick;
        int m_lastMax;
        int m_numMax;
        int m_numCycles;
        int m_freq;
        
        int m_Hscale;
        int m_prevHscale;
        int Hpos;
        int m_Hpos;
        int m_prevHpos;
        
        double m_Vscale;
        double m_prevVscale;
        double m_Vpos;
        double m_prevVpos;
        double m_ampli;
        double m_filter;
        double m_lastData;
        double m_max;
        double m_min;
        
        bool m_reading;
        bool m_newRead;
        bool m_rising;
        bool m_falling;
        bool m_auto;
};

#endif
