/***************************************************************************
 *                                                                         *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef OPAMP2_H
#define OPAMP2_H

//#include "e-op_amp.h"
#include "component.h"
#include "e-source.h"
#include "e-resistor.h"
#include "simulator.h"

#include <QObject>

class LibraryItem;

class MAINMODULE_EXPORT OpAmp2 : public Component, public eElement //, public eOpAmp
{
    Q_OBJECT
    Q_PROPERTY( double Gain       READ gain          WRITE setGain      DESIGNABLE true USER true )
    Q_PROPERTY( bool   Power_Pins READ hasPowerPins  WRITE setPowerPins DESIGNABLE true USER true )
    
    public:

        OpAmp2( QObject* parent, QString type, QString id );
        ~OpAmp2();
        
        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem *libraryItem();

	virtual void initialize();
	virtual void resetState();
	virtual void updateStep();
        virtual void setVChanged();

	double gain();
        void setGain( double gain );
        
        bool hasPowerPins();
        void setPowerPins( bool set );
 
        virtual QPainterPath shape() const;
        virtual void paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget );


     private:
	double m_gain = 1000.0;
	bool m_powerPins = false;
	double m_voltPos = 12.0;
	double m_voltNeg = -12.0;;
	eSource *m_output;
	eResistor *m_rin;
	eResistor *m_rout;
}; 

#endif
