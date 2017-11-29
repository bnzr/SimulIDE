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

#ifndef ELATCHD_H
#define ELATCHD_H

#include "e-logic_device.h"

class MAINMODULE_EXPORT eLatchD : public eLogicDevice
{
    public:

        eLatchD( std::string id, int channels=0 );
        ~eLatchD();

        virtual void initialize();
        virtual void setVChanged();

        //void setInverted( bool inverted );
        void setNumChannels( int channels );
                             
    protected:           
        
};


#endif