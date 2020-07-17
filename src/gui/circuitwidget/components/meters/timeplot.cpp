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
    
    m_area = QRectF( -115, -65, 230, 130 );
    setLabelPos(-100,-80, 0);
    
    m_pin.resize(2);
    m_ePin.resize(2);
    m_pin[0] = new Pin( 180, QPoint(-120,0 ), id+"-PinP", 0, this );
    m_pin[1] = new Pin( 180, QPoint(-120,16), id+"-PinN", 0, this );
    m_ePin[0] = m_pin[0];
    m_ePin[1] = m_pin[1];

    m_pin[0]->setLabelText( "+" );
    m_pin[1]->setLabelText( "_" );
    m_pin[0]->setLabelColor( QColor( 0, 0, 0 ) );
    m_pin[1]->setLabelColor( QColor( 0, 0, 0 ) );
    m_pin[0]->setLength( 5 );
    m_pin[1]->setLength( 5 );
    
    m_timeplotW = new TimeplotWidget( &m_topW );
    m_timeplotW->setupWidget( 116 );
    m_timeplotW->setFixedSize( 220, 120 );
    m_timeplotW->setVisible( true );
    m_timeplotW->setTimeplot( this );
    m_topW.setupWidget( m_timeplotW );
    
    m_proxy = Circuit::self()->addWidget( &m_topW);
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint( -110, -60) );
    //m_proxy->setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent, true );
    
    Simulator::self()->addToUpdateList( this );
}

Timeplot::~Timeplot() 
{
}

void Timeplot::updateStep()
{
    m_timeplotW->read();
    update();
}

void Timeplot::initialize()
{
}

double Timeplot::getVolt()
{
    qDebug() <<m_pin[0]->getVolt() - m_pin[1]->getVolt();
    return m_pin[0]->getVolt() - m_pin[1]->getVolt();
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
    
    p->setBrush( Qt::darkGray );
    p->drawRoundedRect( m_area, 4, 4 );
    
    p->setBrush( Qt::white );
    QPen pen = p->pen();
    pen.setWidth( 0 );
    pen.setColor( Qt::white );
    p->setPen(pen);
    
    p->drawRoundedRect( QRectF( -114, -64, 225, 125 ), 3, 3 );

}

#include "moc_timeplot.cpp"


