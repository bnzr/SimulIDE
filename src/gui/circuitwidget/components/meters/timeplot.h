/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
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

#ifndef TIMEPLOT_H
#define TIMEPLOT_H

#include "component.h"
#include "e-element.h"
#include "timeplotwidget.h"
#include "topwidget.h"
#include "pin.h"

class LibraryItem;

class MAINMODULE_EXPORT Timeplot : public Component, public eElement
{
    Q_OBJECT

    public:

        Timeplot( QObject* parent, QString type, QString id );
        ~Timeplot();

        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem* libraryItem();
        
        virtual void initialize();
        virtual void updateStep();
        
        double getVolt( int i_chan );
        bool channelOn ( int i_chan );
        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget );

    public slots:
        void remove();

    private:
        
        TimeplotWidget* m_timeplotW;
        TopWidget    m_topW;
        QGraphicsProxyWidget* m_proxy;

	int m_component_width;
	int m_component_height;

	int x_pin;
	int y_pin1;
	int y_pin2;
	int y_pin3;
	int y_pin4;
	int y_pin0;

	bool m_input_on [4];
        bool m_input_changed;
};

#endif

