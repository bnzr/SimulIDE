#include "sensor_pt100.h"
#include "connector.h"
#include "circuit.h"
#include "itemlibrary.h"
#include <iostream>
// create properties handling
static const char* SensorPt100_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Value Ohm")
};

// create the Component object construct
Component *SensorPt100::construct (QObject* parent, QString type, QString id)
{
  return new SensorPt100 (parent, type, id);
}

// create an entry with this component in the item library
LibraryItem* SensorPt100::libraryItem()
{
    return new LibraryItem(
        tr( "SensorPt100" ),
        tr( "Sensors Passive" ),
        "sensor_pt100.png",
        "Temperature Sensor PT100",
        SensorPt100::construct );
}

// create the component layout
SensorPt100::SensorPt100 (QObject* parent, QString type, QString id)
  : Component( parent, type, id )
  , eElement( (id+"-eElement").toStdString() )
  , m_pinA (180, QPoint(-20,0), id+"-PinA", 0, this)
  , m_pinB (0, QPoint(20,0), id+"-PinB", 0, this)
  , m_pinM (0, QPoint(0,0), id+"-PinM", 0, this)  // useless , but comply with constructor
  , m_ePinA( (id+"-ePinA").toStdString(), 1 )
  , m_ePinB( (id+"-ePinB").toStdString(), 1 )
  , m_res ((id+"-res").toStdString())
  {
    Q_UNUSED( SensorPt100_properties );

    // connect resistor pins
    m_res.setEpin (0,&m_pinA);
    m_res.setEpin (1,&m_pinB);

    // check pin connections
    m_ePinTst1 = m_res.getEpin (0);
    //std::string ePinId1 = m_ePinTst1->getId();
    //std::cout  << "SensorPt100::SensorPt100" << ePinId1 << std::endl;
    m_ePinTst2 = m_res.getEpin (1);
    //std::string ePinId2 = m_ePinTst2->getId();
    //std::cout  << "SensorPt100::SensorPt100" << ePinId2 << std::endl;
    
    // area of component graphics representation
    m_area = QRectF(-16,-40,32,50);
   
    m_idLabel->setPos(-12,24);
    m_sense_unit = "°C";
    m_unit = "Ω";
    setSense (0);  // start at 0 °C
    setLabelPos(-18,-30, 0);
    setSenseLabelPos(-12, 6, 0);
    setValLabelPos(-16, 40, 0);
    setShowId( false );
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
    m_proxy->setPos( QPoint( -12, 18) );
    
    m_dial = m_dialW.dial;

    
    Simulator::self()->addToUpdateList( this );

    connect( m_dial, SIGNAL(valueChanged(int)),
             this,   SLOT  (senseChanged(int)) );

  }

SensorPt100::~SensorPt100() 
{
}

void SensorPt100::initialize()
{
  m_last_step =  Simulator::self()->step();
  m_resist = sensorFunction (m_sense);
  m_last_resist = m_resist;
  m_new_resist = m_resist;
  setUnit (" ");
  setResist ( m_resist);
  updateStep ();
}

void SensorPt100::updateStep()
{
    m_step = Simulator::self()->step();
    double t = (double) m_step/1e6;
    double dt = t - m_t0_tau;

    m_new_resist = sensorFunction (m_sense);
    double dr = m_new_resist - m_last_resist;
    m_resist = m_last_resist + dr * (1.0 -exp(-dt/m_tau));
    
    setUnit (" ");
    setResist ( m_resist);

    //qDebug() << "SensorPt100::SensorPt100" << m_res.res() << m_res.current()
    //	     << m_ePinTst1->isConnected() << m_ePinTst2->isConnected() ;
   
    m_last_step = m_step;
}

void SensorPt100::remove()
{
    //qDebug() << "SensorPt100::remove enter";
    if( m_ePinTst1->isConnected() ) (static_cast<Pin*>(m_ePinTst1))->connector()->remove();
    if( m_ePinTst2->isConnected() ) (static_cast<Pin*>(m_ePinTst2))->connector()->remove();
    Component::remove();
}


double SensorPt100::getSense ()
{
  return (m_sense);
}

void SensorPt100::setSense( double sense)  // direct update , no time constant
{
    if (sense > m_sense_max) sense = m_sense_max;
    if (sense < m_sense_min) sense = m_sense_min;
    m_sense = sense;
    setSenseUnit (" ");
    setSenseValue(sense);
}

void SensorPt100::senseChanged( int val ) // Called when dial is rotated
{
    //qDebug() <<"SensorPt100::senseChanged" << val;
    double sense = (double)(m_dial->value());
    sense = round(sense/(double)m_sense_step)*(double)m_sense_step;
    //qDebug()<<"SensorPt100::senseChanged dialValue sense"<<m_dial->value()<<sense;
    setSense( sense );
    m_dialW.dial->setValue(m_sense);
    m_t0_tau = (double) (Simulator::self()->step())/1e6;
    m_last_resist = m_resist;
}

double SensorPt100::resist() { return m_value; }

void SensorPt100::setResistSlow( double r )
{
    if( r < 1e-12 ) r = 1e-12;
    Component::setValue( r );       // Takes care about units multiplier
    m_res.setResSafe( m_value*m_unitMult );
}

void SensorPt100::setResist( double r )
{
    if( r < 1e-12 ) r = 1e-12;
    Component::setValue( r );       // Takes care about units multiplier
    m_res.setRes( m_value*m_unitMult );
}

void SensorPt100::setUnit( QString un ) 
{
    Component::setUnit( un );
    m_res.setRes( m_value*m_unitMult );
}

void SensorPt100::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    //p->setBrush(Qt::white);
    //p->drawRoundedRect( QRect( 0, 0, 48, 48 ), 1, 1 );
    //p->setBrush(Qt::darkGray);
    //p->fillRect( QRect( 3, 3, 45, 45 ), Qt::darkGray );

    
    //p->drawRoundedRect( QRect( 8, -56, 8, 40 ), 1, 1 );

    Component::paint( p, option, widget );
    p->drawRect( -16, -4, 32, 8 );
    p->drawImage(QRect(-15, -40, 30, 30),
		 QImage(QString(":/sensor_pt100_thermometer.png")));
}

double SensorPt100::getSenseMin ()
{
  return m_sense_min;
}

double SensorPt100::getSenseMax ()
{
  return m_sense_max;
}

double SensorPt100::getSenseStep ()
{
  return m_sense_step;
}

void SensorPt100::setSenseMin (double s)
{
  m_sense_min = s;
  updateSenseDial();
}

void SensorPt100::setSenseMax (double s)
{
  m_sense_max = s;
  updateSenseDial();
}

void SensorPt100::setSenseStep (double s)
{
  m_sense_step = s;
  updateSenseDial();
}

void SensorPt100::updateSenseDial()
{
    m_dialW.dial->setMinimum(m_sense_min);
    m_dialW.dial->setMaximum(m_sense_max); 
    m_dialW.dial->setValue(m_sense_value*m_sense_unitMult);
    m_dialW.dial->setSingleStep(m_sense_step);
    update();
}

double SensorPt100::sensorFunction (double sense)
{
  // linear approximation
  // double r_sense = r0*(1.0+coef_temp*sense); // linear approximation
  // polynomial approximation (more realistic)
  double r_sense = r0*(1.0+coef_temp_a*sense+coef_temp_b*sense*sense);
  if (sense < 0) {
    r_sense += r0*coef_temp_c*(sense-100)*pow(sense,3.0);
    //qDebug()<<"SensorPt100::sensorFunction"<<sense<<r_sense<<r0*coef_temp_c*(sense-100)*pow(sense,3.0)<<r0*coef_temp_a*sense<<r0*coef_temp_b*sense*sense;
  }
  return r_sense;
}

#include "moc_sensor_pt100.cpp"
