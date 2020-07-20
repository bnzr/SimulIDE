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
    QPoint pinPos = QPoint(-8-20,0);
    m_ePin[0] = new Pin( 180, pinPos, pinId, 0, this);

    pinId = m_id;
    pinId.append(QString("-rPin"));
    pinPos = QPoint(8+20,0);
    m_ePin[1] = new Pin( 0, pinPos, pinId, 1, this);

    m_idLabel->setPos(-12,24);
    m_sense_unit = "MesUnit";
    m_unit = "Ω";
    setSense (1050);
    setLabelPos(-18,-30, 0);
    setSenseLabelPos(-16, 6, 0);
    setValLabelPos(-16, 40, 0);
    setShowId( true );
    setShowSense ( true );
    setShowVal( true );

    m_dialW.setupWidget();
    m_dialW.setFixedSize( 24, 24 ); // 24,24
    m_dialW.dial->setMinimum(m_sense_min);
    m_dialW.dial->setMaximum(m_sense_max); 
    m_dialW.dial->setValue(m_sense_value*m_sense_unitMult);
    m_dialW.dial->setSingleStep(m_sense_step);
    
    m_proxy = Circuit::self()->addWidget( &m_dialW );
    m_proxy->setParentItem( this );
    //m_proxy->setPos( QPoint( -12, -24-5) );
    m_proxy->setPos( QPoint( -12, 18) );
    //m_proxy->setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent, true );

    m_dial = m_dialW.dial;
    
    //Simulator::self()->addToUpdateList( this );
    
    connect( m_dial, SIGNAL(valueChanged(int)),
             this,   SLOT  (senseChanged(int)) );

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sensorTimeRespons()));
    timer->start(100);
}

SensorDummy::~SensorDummy(){}

double SensorDummy::getSense ()
{
  return (m_sense);
}

void SensorDummy::setSense( double sense)  // direct update , no time constant
{
    if (sense > m_sense_max) sense = m_sense_max;
    if (sense < m_sense_min) sense = m_sense_min;
    m_sense = sense;
    setSenseUnit (" ");
    setSenseValue(sense);
    m_resist = sensorFunction (sense);
    m_last_resist = m_resist;
    m_new_resist = m_resist;    
    setUnit (" ");
    setResist ( m_resist);
    qDebug()<<"SensorDummy::setSsense m_resist m_sense "<<m_resist <<m_sense;
    m_last_step =  Simulator::self()->step();
}

void SensorDummy::updateSense( double sense)
{
    if (sense > m_sense_max) sense = m_sense_max;
    if (sense < m_sense_min) sense = m_sense_min;
    m_sense = sense;
    setSenseUnit (" ");
    setSenseValue(sense);
    m_last_resist = m_resist;
    m_new_resist = sensorFunction (sense);
    qDebug()<<"SensorDummy::updateSense m_last_resist m_new_resist" <<  m_last_resist <<  m_new_resist;
    m_last_step =  Simulator::self()->step();
}

void SensorDummy::sensorTimeRespons()
{
    double dr = m_new_resist - m_last_resist;
    m_step = Simulator::self()->step();
    double dt = (double) (m_step-m_last_step)/1e6;
    qDebug() << "update timer dr dt m_step" << dr << dt << m_step;
    m_resist = m_last_resist + dr * (1.0 -exp(-dt/m_tau));
    setUnit (" ");
    setResistFast ( m_resist);
    qDebug()<<"SensorDummy::sensorTimeRespons m_resist m_sense "<<m_resist <<m_sense;
}
  

void SensorDummy::senseChanged( int val ) // Called when dial is rotated
{
    qDebug() <<"SensorDummy::senseChanged" << val;
    double sense = (double)(m_dial->value());
    sense = round(sense/(double)m_sense_step)*(double)m_sense_step;
    qDebug()<<"SensorDummy::senseChanged dialValue sense"<<m_dial->value()<<sense;
    m_dialW.dial->setValue(sense);
    updateSense( sense );
}

double SensorDummy::resist() { return m_value; }

void SensorDummy::setResist( double r )
{
    if( r < 1e-12 ) r = 1e-12;
    qDebug() <<"SensorDummy::setResist r "<<r;
    Component::setValue( r );       // Takes care about units multiplier
    qDebug() <<"SensorDummy::setResist  m_value m_unitMult "<<m_value<<m_unitMult;
    eResistor::setResSafe( m_value*m_unitMult );
    qDebug() <<"SensorDummy::setResist eResistor res "<<eResistor::res();
}

void SensorDummy::setResistFast( double r )
{
    if( r < 1e-12 ) r = 1e-12;
    qDebug() <<"SensorDummy::setResist r "<<r;
    Component::setValue( r );       // Takes care about units multiplier
    qDebug() <<"SensorDummy::setResist  m_value m_unitMult "<<m_value<<m_unitMult;
    eResistor::setRes( m_value*m_unitMult );
    qDebug() <<"SensorDummy::setResist eResistor res "<<eResistor::res();
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

    // draw the sensor symbol
    // example a resistor modified by incoming flux (arraows)
    p->drawRect( -10.5, -4, 21, 8 );
    p->drawLine( -20, 0, -10.5, 0 );
    p->drawLine( 20, 0, 12.5, 0 );
    double x0, y0;
    x0 = -9;
    y0 = -7;
    for (int i=0; i<3; i++)
    {	
       p->drawLine( x0, y0, x0,  y0-3);
       p->drawLine( x0, y0, x0+3,  y0);
       p->drawLine( x0, y0, x0+6,  y0-6);
       p->drawLine( x0+6, y0-6, x0+2,  y0-6);
       p->drawLine( x0+2, y0-6, x0+6,  y0-2*6);
       x0 += 8.0;
    }

}

double SensorDummy::sensorFunction (double sense)
{
  const double a = 0.25;
  const double b = 10.0;
  double r_sense = a*sense+b;
    qDebug()<<"SensorDummy::sensorFunction"<<sense<<r_sense;
  return r_sense;
}

#include "moc_sensor_dummy.cpp"
