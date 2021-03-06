/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#ifndef PROCESSOR_H
#define PROCESSOR_H

//#include <QtGui>

#include "ramtable.h"
#include "terminalwidget.h"

class RamTable;

class MAINMODULE_EXPORT BaseProcessor : public QObject
{
    Q_OBJECT
    public:
        BaseProcessor( QObject* parent=0 );
        ~BaseProcessor();
        
 static BaseProcessor* self() { return m_pSelf; }
 
        QString getFileName();

        virtual void    setDevice( QString device );
        virtual QString getDevice();
        
        virtual void setDataFile( QString datafile );

        virtual bool loadFirmware( QString file )=0;
        virtual bool getLoadStatus() { return m_loadStatus; }
        virtual void terminate();

        virtual void setSteps( int steps );
        virtual void step()=0;
        virtual void stepOne()=0;
        virtual void reset()=0;
        virtual int  pc()=0;

        virtual int getRamValue( QString name );
        virtual int getRamValue( int address )=0;
        virtual int getRegAddress( QString name );
        virtual void addWatchVar( QString name, int address, QString type );
        virtual void updateRamValue( QString name );
        
        virtual void setUsart( bool usart ) { m_usartTerm = usart; }
        virtual void setSerPort( bool serport ) { m_serialPort = serport; }
        virtual void uartOut( uint32_t value );
        virtual void uartIn( uint32_t value );
        
        virtual void initialized();
    
    protected:
 static BaseProcessor* m_pSelf;
 
        virtual void setRegisters();
        virtual int  validate( int address )=0;

        QString m_symbolFile;
        QString m_dataFile;
        QString m_device;
        
        int  m_mcuStepsPT;
        unsigned long m_nextCycle;

        RamTable* m_ramTable;
        QHash<QString, int> m_regsTable;     // int max 32 bits
        QHash<QString, float> m_floatTable;  // float 32 bits
        QHash<QString, QString> m_typeTable;

        bool m_loadStatus;
        bool m_usartTerm;
        bool m_serialPort;
};


#endif

