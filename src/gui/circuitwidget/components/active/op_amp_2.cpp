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

#include "op_amp_2.h"
//#include "e-source.h"
#include "itemlibrary.h"
#include "connector.h"
#include "pin.h"

static const char* OpAmp2_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Power Pins")
};

Component* OpAmp2::construct( QObject* parent, QString type, QString id )
{
        return new OpAmp2( parent, type, id );
}

LibraryItem* OpAmp2::libraryItem()
{
    return new LibraryItem(
        tr( "OpAmp2" ),
        tr( "Active" ),
        "opamp.png",
        "opAmp2",
        OpAmp2::construct );
}

OpAmp2::OpAmp2( QObject* parent, QString type, QString id )
  : Component( parent, type, id )
  , eElement ( (id+"-eElement").toStdString() )
{
    Q_UNUSED( OpAmp2_properties );
    
    m_area = QRect( -18, -8*2, 36, 8*2*2 );
    setLabelPos(-16,-32, 0);
    
    setGain( 10.0 );
    m_voltPos = 12;
    m_voltNeg = -12;

    setNumEpins(7);
    m_pin.resize( 7 );
    //m_gain = 1000;

    QString newId = id;
    
    newId.append(QString("-inputNinv"));
    m_pin[0] = new Pin( 180, QPoint(-16-8,-8), newId, 0, this );
    m_pin[0]->setLabelText( "+" );
    m_pin[0]->setLabelColor( QColor( 0, 0, 0 ) );
    m_ePin[0] = m_pin[0];
    
    newId = id;
    newId.append(QString("-inputInv"));
    m_pin[1] = new Pin( 180, QPoint(-16-8,8), newId, 1, this );
    m_pin[1]->setLabelText( " -" );
    m_pin[1]->setLabelColor( QColor( 0, 0, 0 ) );
    m_ePin[1] = m_pin[1];
    
    newId = id;
    newId.append(QString("-output"));
    m_pin[2] = new Pin( 0, QPoint(16+8,0), newId, 2, this );
    m_ePin[2] = m_pin[2];
    //newId.append("-eSource");
    //m_output = new eSource( newId.toStdString(), m_ePin[2] );
    //////m_output->setImp( 40 );
    //m_output->setOut( true );
    
    newId = id;
    newId.append(QString("powerPos"));
    m_pin[3] = new Pin( 90, QPoint(0,-16), newId, 3, this );
    m_ePin[3] = m_pin[3];
    //newId.append("-eSource");
    //m_powerPos = new eSource( newId.toStdString(), m_ePin[2] );
    
    newId = id;
    newId.append(QString("powerNeg"));
    m_pin[4] = new Pin( 270, QPoint(0, 16), newId, 4, this );
    m_ePin[4] = m_pin[4];
    //newId.append("-eSource");
    //m_powerNeg = new eSource( newId.toStdString(), m_ePin[3] );

    setPowerPins( false );

    // dummy pin with gain
    newId = id;
    newId.append(QString("-outgain"));
    m_pin[5] = new Pin( 0, QPoint(16+8,8), newId, 5, this );
    m_ePin[5] = m_pin[5];
    newId.append("-eSource");
    m_output = new eSource( newId.toStdString(), m_ePin[5] );
    ////m_output->setImp( 40 );
    m_output->setOut( true );
    m_pin[5]->setEnabled( true );
    m_pin[5]->setVisible( true );


    // dummy pin with out r
    newId = id;
    newId.append(QString("-outr"));
    m_pin[6] = new Pin( 0, QPoint(16+8,16), newId, 6, this );
    m_ePin[6] = m_pin[6];
 

    // input impendance (resistive only)
    newId = id;
    newId.append(QString("-RIn"));
    m_rin = new eResistor(newId.toStdString());
    m_rin->setRes(1000000);
    ePin* rin_pin0 = m_ePin[0];
    m_rin->setEpin (0,rin_pin0);
    ePin* rin_pin1 = m_ePin[1];
    m_rin->setEpin (1,rin_pin1);
    
    // output impendance (resistive only)
    newId = id;
    newId.append(QString("-ROut"));
    m_rout = new eResistor(newId.toStdString());
    m_rout->setRes(10);
    ePin* rout_pin0 = m_ePin[2];
    m_rout->setEpin (1,rout_pin0);
    ePin* rout_pin1 = m_ePin[6];
    m_rout->setEpin (0,rout_pin1);
    

 
    Simulator::self()->addToUpdateList( this );
    //Simulator::self()->addToChangedFast( this );
}
OpAmp2::~OpAmp2()
{
}

void OpAmp2::initialize()
{
  qDebug() << "OpAmp2::initialize" << m_ePin[0]->isConnected()<< m_ePin[1]->isConnected();
  eNode* enod = m_pin[5]->getEnode();
  qDebug() << "OpAmp2::initialize" << enod->getVolt();
  //m_ePin[0]->setEnodeComp( m_ePin[1]->getEnode() );
  //m_ePin[1]->setEnodeComp( m_ePin[0]->getEnode() );
  if( m_ePin[0]->isConnected() ) m_ePin[0]->getEnode()->addToNoLinList(this);
  if( m_ePin[1]->isConnected() ) m_ePin[1]->getEnode()->addToNoLinList(this);
  if( m_ePin[2]->isConnected() ) m_ePin[2]->getEnode()->addToNoLinList(this);
  if( m_ePin[5]->isConnected() ) m_ePin[5]->getEnode()->addToNoLinList(this);
}

void OpAmp2::resetState()
{
  qDebug() << "OpAmp2::resetState";
}

void OpAmp2::updateStep()
{
  double vd = m_ePin[0]->getVolt()-m_ePin[1]->getVolt();
  double vo = m_gain*vd;
  if (vo > m_voltPos) vo = m_voltPos;
  if (vo < m_voltNeg) vo = m_voltNeg;
  m_output->setVoltHigh(vo);
  //m_output->stampOutput();
  m_output->stamp();
  //qDebug() << "OpAmp2::updateStep" << vd << vo << m_ePin[0]->getVolt() << m_ePin[1]->getVolt() << m_ePin[2]->getVolt() << m_ePin[5]->getVolt();
  eNode* enod = m_pin[5]->getEnode();
  qDebug() << "OpAmp2::updateStep" << enod->getVolt();
}

void OpAmp2::setVChanged()
{
  //qDebug() << "OpAmp2::setVChanged";
  updateStep();
}


double OpAmp2::gain()                {return m_gain;}
void   OpAmp2::setGain( double gain ){m_gain = gain;}

bool OpAmp2::hasPowerPins()          {return m_powerPins;}

void OpAmp2::setPowerPins( bool set )
{
    m_pin[3]->setEnabled( set );
    m_pin[3]->setVisible( set );
    m_pin[4]->setEnabled( set );
    m_pin[4]->setVisible( set );
    
    m_powerPins = set;
}

QPainterPath OpAmp2::shape() const
{
    QPainterPath path;
    
    QVector<QPointF> points;
    
    points << QPointF(-16,-16 )
           << QPointF(-16, 16 )
           << QPointF( 16,  1 )
           << QPointF( 16, -1 );
        
    path.addPolygon( QPolygonF(points) );
    path.closeSubpath();
    return path;
}

void OpAmp2::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );

    QPen pen = p->pen();
    pen.setWidth(2);
    p->setPen(pen);

    static const QPointF points[4] = {
        QPointF(-16,-16 ),
        QPointF(-16, 16 ),
        QPointF( 16,  1 ),
        QPointF( 16, -1 )            };

    p->drawPolygon(points, 4);
}

#include "moc_op_amp_2.cpp"
