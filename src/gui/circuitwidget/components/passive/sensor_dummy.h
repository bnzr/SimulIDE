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

/* 2020/07 : Canvas for resistive passive sensors 
   Added by benblop@gmail.com  
*/

#ifndef SENSORDUMMY_H
#define SENSORDUMMY_H

#include "itemlibrary.h"
#include "e-resistor.h"
#include "pin.h"
#include "dialwidget.h"


class MAINMODULE_EXPORT SensorDummy : public Component, public eResistor
{
    Q_OBJECT
    //Q_PROPERTY( double Resistance READ resist   WRITE setResist  DESIGNABLE true USER true )
    //Q_PROPERTY( QString  Unit     READ unit     WRITE setUnit    DESIGNABLE true USER true )
    //Q_PROPERTY( bool     Show_res READ showVal  WRITE setShowVal DESIGNABLE true USER true )
    Q_PROPERTY( double Sense READ getSense   WRITE setSense  DESIGNABLE true USER true )
    Q_PROPERTY( bool     Show_sense READ showSense  WRITE setShowSense DESIGNABLE true USER true )

    public:
        //QRectF boundingRect() const { return QRectF( -11, -4.5, 22, 9 ); }
        QRectF boundingRect() const { return QRectF( -25, -25, 50, 50 ); }

        SensorDummy( QObject* parent, QString type, QString id );
        ~SensorDummy();

        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem *libraryItem();

        double resist();
        void setResist( double r );
        void setResistFast( double r );
	double sensorFunction ( double sense );

	double getSense();
	void setSense( double sense );
 	void updateSense( double sense );
       
        void setUnit( QString un );

        virtual void paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget );

    public slots:
        void senseChanged( int val );
	void sensorTimeRespons ();
        void remove();

    private:
	uint64_t m_step;
	uint64_t m_last_step;
	double m_resist;
	double m_new_resist;
	double m_last_resist;
        double m_sense;
	double m_tau = 0.25;
	
	int m_sense_min = 50;
	int m_sense_max = 2000;
	int m_sense_step = 50;
        DialWidget m_dialW;	
        QDial* m_dial;
        QGraphicsProxyWidget* m_proxy;
};

#endif
