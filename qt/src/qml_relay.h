//==========================================================================
//
//==========================================================================
//  Copyright (c) DogRatIan.  All rights reserved.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//==========================================================================
// Naming conventions
// ~~~~~~~~~~~~~~~~~~
//                Class : Leading C
//               Struct : Leading T
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//==========================================================================
#ifndef _INC_QML_RELAY_H
#define _INC_QML_RELAY_H

//==========================================================================
//==========================================================================
#include <QObject>
#include <QString>
#include <QList>

#include "worker.h"

//==========================================================================
// Defines
//==========================================================================

//==========================================================================
//==========================================================================
class CRelay : public QObject
{
    Q_OBJECT

public:
    explicit CRelay (QObject *aParent = 0);
     ~CRelay (void);

    Q_INVOKABLE void stop (void);
    Q_INVOKABLE int testPort (QString aPortName);
    Q_INVOKABLE int openPort (QString aPortName);
    Q_INVOKABLE int setRelay (int aIdx, int aValue);

    void handleWorkerMessage (const QString aMsg);
    void handleWorkerActionUpdate (int aAction, int aResult, const QString aMsg);

signals:
    void message (const QString aMessage);
    void testDone (int aResult, const QString aMessage);
    void connected (QString aRelayId);
    void disconnected (void);
    void relay1Changed (int aStatus);
    void relay2Changed (int aStatus);
    void relay3Changed (int aStatus);
    void relay4Changed (int aStatus);

private:
    CWorker worker;
};

//==========================================================================
//==========================================================================
#endif
