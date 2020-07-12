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

#include "sensor_dummy.h"
#include "connector.h"
#include "itemlibrary.h"
#include "circuit.h"

static const char* SensorDummy_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Resistance"),
    QT_TRANSLATE_NOOP("App::Property","Show res")
};

Component* SensorDummy::construct( QObject* parent, QString type, QString id )
{ return new SensorDummy( parent, type, id ); }

LibraryItem* SensorDummy::libraryItem()
{
    return new LibraryItem(
	    tr( "SensorDummy" ),  // sensor class name
            tr( "Sensors Passive" ),      // category in GUI where the sensor icon is placed
            "sensor_dummy.png",   // icon file
            "SensorDummy",
            SensorDummy::construct);
}

SensorDummy::SensorDummy( QObject* parent, QString type, QString id )
        : Component( parent, type, id )
        , eResistor( id.toStdString() )
{
    Q_UNUSED( SensorDummy_properties );
    QString pinId = m_id;
    pinId.append(QString("-lPin"));
    QPoint pinPos = QPoint(-8-8,0);
    m_ePin[0] = new Pin( 180, pinPos, pinId, 0, this);

    pinId = m_id;
    pinId.append(QString("-rPin"));
    pinPos = QPoint(8+8,0);
    m_ePin[1] = new Pin( 0, pinPos, pinId, 1, this);

    m_idLabel->setPos(-12,-24);
    setLabelPos(-12,-20, 0);
    
    m_unit = "Ω";
    setResist( m_resist );
    
    setValLabelPos(-16, 6, 0);
    setShowVal( true );

    m_sense_unit = "Lux";
    setSenseLabelPos(-16, 26, 0);
    setSenseValue(1200);
    setShowSense ( true );
    
}

SensorDummy::~SensorDummy(){}

double SensorDummy::resist() { return m_value; }

void SensorDummy::setResist( double r )
{
    if( r < 1e-12 ) r = 1e-12;
    
    Component::setValue( r );       // Takes care about units multiplier
    eResistor::setResSafe( m_value*m_unitMult );
}

void SensorDummy::setUnit( QString un ) 
{
    Component::setUnit( un );
    eResistor::setResSafe( m_value*m_unitMult );
}

void SensorDummy::remove()
{
    if( m_ePin[0]->isConnected() ) (static_cast<Pin*>(m_ePin[0]))->connector()->remove();
    if( m_ePin[1]->isConnected() ) (static_cast<Pin*>(m_ePin[1]))->connector()->remove();
    Component::remove();
}

void SensorDummy::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    //qDebug()<<"SensorDummy::paint p"<<p; 
    Component::paint( p, option, widget );
    
    p->drawRect( -10.5, -4, 21, 8 );


    QFont font = p->font();
    font.setPixelSize(12);
    p->setFont(font);


}

#include "moc_sensor_dummy.cpp"
