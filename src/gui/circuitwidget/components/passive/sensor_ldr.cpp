#include "sensor_ldr.h"
#include "connector.h"
#include "circuit.h"
#include "itemlibrary.h"
#include <iostream>
// create properties handling
static const char* SensorLdr_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Value Ohm")
};

// create the Component object construct
Component *SensorLdr::construct (QObject* parent, QString type, QString id)
{
  return new SensorLdr (parent, type, id);
}

// create an entry with this component in the item library
LibraryItem* SensorLdr::libraryItem()
{
    return new LibraryItem(
        tr( "Light Sensor LDR" ),
        tr( "Sensors Passive" ),
        "sensor_ldr.png",
        "SensorLdr",
        SensorLdr::construct );
}

// create the component layout
SensorLdr::SensorLdr (QObject* parent, QString type, QString id)
  : Component( parent, type, id )
  , eElement( (id+"-eElement").toStdString() )
  , m_pinA (180, QPoint(-20,0), id+"-PinA", 0, this)
  , m_pinB (0, QPoint(20,0), id+"-PinB", 0, this)
  , m_pinM (0, QPoint(0,0), id+"-PinM", 0, this)  // useless , but comply with constructor
  , m_ePinA( (id+"-ePinA").toStdString(), 1 )
  , m_ePinB( (id+"-ePinB").toStdString(), 1 )
  , m_res ((id+"-res").toStdString())
  {
    Q_UNUSED( SensorLdr_properties );

    // connect resistor pins
    m_res.setEpin (0,&m_pinA);
    m_res.setEpin (1,&m_pinB);

    // check pin connections
    m_ePinTst1 = m_res.getEpin (0);
    //std::string ePinId1 = m_ePinTst1->getId();
    //std::cout  << "SensorLdr::SensorLdr" << ePinId1 << std::endl;
    m_ePinTst2 = m_res.getEpin (1);
    //std::string ePinId2 = m_ePinTst2->getId();
    //std::cout  << "SensorLdr::SensorLdr" << ePinId2 << std::endl;
    
    // area of component graphics representation
    m_area = QRectF(-16,-40,32,50);
   
    m_idLabel->setPos(-12,24);
    m_sense_unit = "lux";
    m_unit = "Ω";
    setSense (1050);
    setLabelPos(-18,-30, 0);
    setSenseLabelPos(-12, 6, 0);
    setValLabelPos(-16, 40, 0);
    setShowId( false );
    setShowSense ( true );
    setShowVal( false );

    m_dialW.setupWidget();
    m_dialW.setFixedSize( 24, 24 ); // 24,24
    m_dialW.dial->setMinimum(m_sense_min);
    m_dialW.dial->setMaximum(m_sense_max); 
    m_dialW.dial->setValue(m_sense_value*m_sense_unitMult);
    m_dialW.dial->setSingleStep(m_sense_step);
    
    m_proxy = Circuit::self()->addWidget( &m_dialW );
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint( -12, 18) );
    
    m_dial = m_dialW.dial;

    
    Simulator::self()->addToUpdateList( this );

    connect( m_dial, SIGNAL(valueChanged(int)),
             this,   SLOT  (senseChanged(int)) );

  }

SensorLdr::~SensorLdr() 
{
}

void SensorLdr::initialize()
{
  m_last_step =  Simulator::self()->step();
  m_resist = sensorFunction (m_sense);
  m_last_resist = m_resist;
  m_new_resist = m_resist;
  setUnit (" ");
  setResist ( m_resist);
  updateStep ();
}

void SensorLdr::updateStep()
{
    m_step = Simulator::self()->step();
    double dt = (double) (m_step-m_last_step)/1e6;

    m_last_resist = m_resist;
    m_new_resist = sensorFunction (m_sense);
    double dr = m_new_resist - m_last_resist;
    m_resist = m_last_resist + dr * (1.0 -exp(-dt/m_tau));

    setUnit (" ");
    setResist ( m_resist);

    //qDebug() << "SensorLdr::SensorLdr" << m_res.res() << m_res.current()
    //	     << m_ePinTst1->isConnected() << m_ePinTst2->isConnected() ;
   
    m_last_step = m_step;
}

void SensorLdr::remove()
{
    //qDebug() << "SensorLdr::remove enter";
    if( m_ePinTst1->isConnected() ) (static_cast<Pin*>(m_ePinTst1))->connector()->remove();
    if( m_ePinTst2->isConnected() ) (static_cast<Pin*>(m_ePinTst2))->connector()->remove();
    Component::remove();
}


double SensorLdr::getSense ()
{
  return (m_sense);
}

void SensorLdr::setSense( double sense)  // direct update , no time constant
{
    if (sense > m_sense_max) sense = m_sense_max;
    if (sense < m_sense_min) sense = m_sense_min;
    m_sense = sense;
    setSenseUnit (" ");
    setSenseValue(sense);
}

void SensorLdr::senseChanged( int val ) // Called when dial is rotated
{
    //qDebug() <<"SensorLdr::senseChanged" << val;
    double sense = (double)(m_dial->value());
    sense = round(sense/(double)m_sense_step)*(double)m_sense_step;
    //qDebug()<<"SensorLdr::senseChanged dialValue sense"<<m_dial->value()<<sense;
    setSense( sense );
    m_dialW.dial->setValue(m_sense);
}

double SensorLdr::resist() { return m_value; }

void SensorLdr::setResistSlow( double r )
{
    if( r < 1e-12 ) r = 1e-12;
    Component::setValue( r );       // Takes care about units multiplier
    m_res.setResSafe( m_value*m_unitMult );
}

void SensorLdr::setResist( double r )
{
    if( r < 1e-12 ) r = 1e-12;
    Component::setValue( r );       // Takes care about units multiplier
    m_res.setRes( m_value*m_unitMult );
}

void SensorLdr::setUnit( QString un ) 
{
    Component::setUnit( un );
    m_res.setRes( m_value*m_unitMult );
}

void SensorLdr::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    //p->setBrush(Qt::white);
    //p->drawRoundedRect( QRect( 0, 0, 48, 48 ), 1, 1 );
    //p->setBrush(Qt::darkGray);
    //p->fillRect( QRect( 3, 3, 45, 45 ), Qt::darkGray );

    
    //p->drawRoundedRect( QRect( 8, -56, 8, 40 ), 1, 1 );

    Component::paint( p, option, widget );
    p->drawRect( -16, -4, 32, 8 );
    p->drawImage(QRect(-15, -40, 30, 30),
		 QImage(QString(":/sensor_ldr_illumination.png")));
}

double SensorLdr::sensorFunction (double sense)
{
  double r_sense =  r1*pow (sense,-gamma);
    //qDebug()<<"SensorLdr::sensorFunction"<<sense<<r_sense;
  return r_sense;
}

#include "moc_sensor_ldr.cpp"
