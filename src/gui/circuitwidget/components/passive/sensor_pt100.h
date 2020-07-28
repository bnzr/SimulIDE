#ifndef SENSORPT100_H
#define SENSORPT100_H

#include "component.h"
#include "e-resistor.h"
#include "e-element.h"
#include "dialwidget.h"
#include "pin.h"

class LibraryItem;

class MAINMODULE_EXPORT SensorPt100 : public Component, public eElement
{
    Q_OBJECT
    //Q_PROPERTY( double  Resistance READ res     WRITE setRes    DESIGNABLE true USER true )
    //Q_PROPERTY( QString Unit       READ unit    WRITE setUnit    DESIGNABLE true USER true )
    //Q_PROPERTY( bool    Show_res   READ showVal WRITE setShowVal DESIGNABLE true USER true )
    //Q_PROPERTY( int     Value_Ohm  READ val     WRITE setVal    DESIGNABLE true USER true )
    Q_PROPERTY( double Sense READ getSense   WRITE setSense  DESIGNABLE true USER true )
    Q_PROPERTY( bool     Show_sense READ showSense  WRITE setShowSense DESIGNABLE true USER true )

 public:
    SensorPt100( QObject* parent, QString type, QString id );
    ~SensorPt100();

    static Component* construct( QObject* parent, QString type, QString id );
    static LibraryItem* libraryItem();

    virtual void initialize();
    virtual void updateStep();
    virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget );

    double getSense();
    void setSense( double sense );
    void setUnit( QString un );
    double resist();
    void setResist( double r );
    void setResistSlow( double r );
    double sensorFunction ( double sense );
    
    public slots:
      void senseChanged( int val );
      virtual void remove();
      
 private:
    Pin m_pinA;
    Pin m_pinB;
    Pin m_pinM;
    ePin m_ePinA;
    ePin m_ePinB;
    ePin* m_ePinTst1;
    ePin* m_ePinTst2;
    
    eResistor m_res;

    int m_sense_min = -50;
    int m_sense_max = 250;
    int m_sense_step = 10;
    DialWidget m_dialW;	
    QDial* m_dial;
    QGraphicsProxyWidget* m_proxy;

    uint64_t m_step;
    uint64_t m_last_step;
    double m_resist;
    double m_new_resist;
    double m_last_resist;
    double m_sense;
    double m_tau = 0.3;  // 0.3 in water 4.0 in air
    double m_t0_tau = 0.0;
    double r0 = 100.0;
    double coef_temp = 0.00385;
    
};

#endif
