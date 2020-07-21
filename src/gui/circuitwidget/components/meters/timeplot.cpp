/***************************************************************************
 *   Copyright (C) 2018 by santiago González                               *
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

#include "timeplot.h"
#include "connector.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "circuitwidget.h"
#include "timeplotwidget.h"

static const char* Timeplot_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Filter")
};

Component* Timeplot::construct( QObject* parent, QString type, QString id )
{
    return new Timeplot( parent, type, id );
}

LibraryItem* Timeplot::libraryItem()
{
    return new LibraryItem(
        tr( "Timeplot" ),
        tr( "Meters" ),
        "timeplot.png",
        "Timeplot",
        Timeplot::construct );
}

Timeplot::Timeplot( QObject* parent, QString type, QString id )
      : Component( parent, type, id )
      , eElement( (id+"-eElement").toStdString() )
      , m_topW( )
{
    Q_UNUSED( Timeplot_properties );
    qDebug() << "Timeplot::Timeplot get types" << typeid(m_topW).name() << typeid(*this).name() ; 

    m_component_width = 220;
    m_component_height = 120;
    m_area = QRectF(0, 0, m_component_width, m_component_height);
    setLabelPos((m_component_width*1)/4,-16, 0);
    setShowId( true );
    
    m_pin.resize(5);
    m_ePin.resize(5);

    x_pin = -10;
    y_pin1 = 8;
    y_pin2 = 8+24;
    y_pin3 = 8+24*2;
    y_pin4 = 8+24*3;
    y_pin0 = 8+24*4;

    m_pin[1] = new Pin( 180, QPoint(x_pin,y_pin1), id+"-PinS1", 0, this );
    m_pin[2] = new Pin( 180, QPoint(x_pin,y_pin2), id+"-PinS2", 0, this );
    m_pin[3] = new Pin( 180, QPoint(x_pin,y_pin3), id+"-PinS3", 0, this );
    m_pin[4] = new Pin( 180, QPoint(x_pin,y_pin4), id+"-PinS4", 0, this );
    m_pin[0] = new Pin( 180, QPoint(x_pin,y_pin0), id+"-PinN", 0, this );

    for (int i_pin=0; i_pin<5; i_pin++)
    {
        m_ePin[i_pin] = m_pin[i_pin];
    }
    
    m_pin[1]->setLabelText( "+S1" );
    m_pin[2]->setLabelText( "+S2" );
    m_pin[3]->setLabelText( "+S3" );
    m_pin[4]->setLabelText( "+S4" );
    m_pin[0]->setLabelText( "- (Gnd)" );
    
    for (int i_pin=0; i_pin<5; i_pin++)
    {
        m_pin[i_pin]->setLabelColor( QColor( 0, 0, 0 ) );
	m_pin[i_pin]->setLength( 10 );
    }

    for (int i_inp=0; i_inp<4; i_inp++) m_input_on[i_inp] = false;
    m_input_changed = true;
    
    m_timeplotW = new TimeplotWidget( &m_topW );
    m_timeplotW->setupWidget( );
    m_timeplotW->setVisible( true );
    m_timeplotW->setTimeplot( this );
    m_topW.setupWidget( m_timeplotW );
    
    m_proxy = Circuit::self()->addWidget( &m_topW);
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint(  m_component_width-10-60, 10) );
    //m_proxy->setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent, true );
    
    Simulator::self()->addToUpdateList( this );
}

Timeplot::~Timeplot() 
{
}


void Timeplot::updateStep()
{
   //qDebug() << "Timeplot::updateStep" ;
    m_input_changed = false;
    for (int i_inp=0; i_inp<4; i_inp++)
    {
	bool last_v =  m_input_on[i_inp];
	bool new_v = false;
	if (m_ePin[i_inp+1]->isConnected() && m_ePin[0]->isConnected())
	{
	    new_v = true;
	}
	if (new_v != last_v) m_input_changed = true;
	m_input_on[i_inp] = new_v;
    }
  m_timeplotW->read();
  update();
}

void Timeplot::initialize()
{
  // qDebug() << "Timeplot::initialize" ;
}

bool Timeplot::channelOn( int i_chan)
{
    return m_input_on[i_chan-1];
}

double Timeplot::getVolt(int i_chan)
{
  //qDebug() << "Timeplot::getVolt" << i_chan << m_pin[i_chan]->getVolt() - m_pin[0]->getVolt();
    return m_pin[i_chan]->getVolt() - m_pin[0]->getVolt();
}

void Timeplot::remove()
{    
    Simulator::self()->remFromUpdateList( this );
    m_timeplotW->setTimeplot( 0l );
    
    Component::remove();
}

void Timeplot::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );

    //qDebug() << "Timeplot::paint pin1" << m_input_changed;
    p->setBrush( Qt::lightGray);
    p->drawRoundedRect( m_area, 4, 4 );
    
    //p->setBrush( Qt::white );
    //QPen pen = p->pen();
    //pen.setWidth( 0 );
    //pen.setColor( Qt::white );
    //p->setPen(pen);

    p->setBrush( Qt::darkRed );
    if (m_input_on[0]) p->setBrush( Qt::red );
    p->drawRoundedRect( QRectF( 25, y_pin1-5, 20, 10 ), 1, 1 );
    p->setBrush( Qt::darkBlue );
    if (m_input_on[1]) p->setBrush( Qt::blue );
    p->drawRoundedRect( QRectF( 25, y_pin2-5, 20, 10 ), 1, 1 );
    p->setBrush( Qt::darkGreen);
    if (m_input_on[2]) p->setBrush( Qt::green );
    p->drawRoundedRect( QRectF( 25, y_pin3-5, 20, 10 ), 1, 1 );
    p->setBrush( Qt::darkCyan );
    if (m_input_on[3]) p->setBrush( Qt::cyan );
    p->drawRoundedRect( QRectF( 25, y_pin4-5, 20, 10 ), 1, 1 );
}

#include "moc_timeplot.cpp"


