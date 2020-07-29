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

#include "connector.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "fixcursource.h"

static const char* FixCurSource_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Current"),
    QT_TRANSLATE_NOOP("App::Property","Show Amp")
};


Component* FixCurSource::construct( QObject* parent, QString type, QString id )
{ return new FixCurSource( parent, type, id ); }

LibraryItem* FixCurSource::libraryItem()
{
    return new LibraryItem(
        tr( "Fixed Current" ),
        tr( "Sources" ),
        "voltage.png",
        "Fixed Current",
        FixCurSource::construct );
}

FixCurSource::FixCurSource( QObject* parent, QString type, QString id )
          : Component( parent, type, id )
          , eElement( id.toStdString() )
{
    Q_UNUSED( FixCurSource_properties );
    
    setLabelPos(-64,-24 );
    
    m_changed = false;

    QString nodid = id;
    nodid.append(QString("-outPin"));
    QPoint nodpos = QPoint(16,0);
    m_outpin = new Pin( 0, nodpos, nodid, 0, this);
    
    m_unit = "A";
    setCurrent(0.01);
    setValLabelPos(-16, 8 , 0 ); // x, y, rot 
    setShowVal( true );

    m_button = new QPushButton( );
    m_button->setMaximumSize( 16,16 );
    m_button->setGeometry(-20,-16,16,16);
    m_button->setCheckable( true );

    m_proxy = Circuit::self()->addWidget( m_button );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint(-32, -8) );
    
    Simulator::self()->addToUpdateList( this );

    connect( m_button, SIGNAL( clicked() ),
             this,     SLOT  ( onbuttonclicked() ));
}

FixCurSource::~FixCurSource() 
{
    //delete m_out;
}

void FixCurSource::onbuttonclicked()
{
    m_changed = true;
}

void FixCurSource::updateStep()
{
    if( m_changed ) 
    {
        double current = 0;
        if( m_button->isChecked() ) current = m_current;
        m_outpin->stampCurrent( current );
        m_changed = false;
        update();
    }
}

double FixCurSource::current()  
{ 
    return m_current; 
}

void FixCurSource::setCurrent( double a )
{
    Component::setValue( a );       // Takes care about units multiplier
    m_current = m_value*m_unitMult;
    m_changed = true;
}

void FixCurSource::setUnit( QString un ) 
{
    Component::setUnit( un );
    m_current = m_value*m_unitMult;
    m_changed = true;
}

void FixCurSource::remove()
{
    if( m_outpin->isConnected() ) m_outpin->connector()->remove();

    Simulator::self()->remFromUpdateList( this );
    
    Component::remove();
}


void FixCurSource::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );

    if ( m_button->isChecked())
        p->setBrush( QColor( 255, 166, 0 ) );
    else
        p->setBrush( QColor( 230, 230, 255 ) );

    p->drawRoundedRect( QRectF( -8, -8, 16, 16 ), 2, 2);
}

#include "moc_fixcursource.cpp"

