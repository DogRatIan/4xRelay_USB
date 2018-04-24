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

#include "debug.h"

#include "qml_relay.h"

//==========================================================================
//==========================================================================


//==========================================================================
//==========================================================================
// Public members
// vvvvvvvvvvvvvv
//==========================================================================
//==========================================================================

//==========================================================================
// Constructor
//==========================================================================
CRelay::CRelay (QObject *aParent) :
    QObject(aParent)
{

    // Connect worker signals
    connect (&worker, &CWorker::message, this, &CRelay::handleWorkerMessage);
    connect (&worker, &CWorker::actionUpdate, this, &CRelay::handleWorkerActionUpdate);
    connect (&worker, &CWorker::relay1Changed, this, &CRelay::relay1Changed);
    connect (&worker, &CWorker::relay2Changed, this, &CRelay::relay2Changed);
    connect (&worker, &CWorker::relay3Changed, this, &CRelay::relay3Changed);
    connect (&worker, &CWorker::relay4Changed, this, &CRelay::relay4Changed);

    // Start worker
    worker.start ();
}

//==========================================================================
// Destroyer
//==========================================================================
CRelay::~CRelay (void)
{
    // Stop worker
    worker.requestAbort();
    worker.wait (1000);

    // Disconnect worker signals
    disconnect (&worker, &CWorker::message, this, &CRelay::handleWorkerMessage);
    disconnect (&worker, &CWorker::actionUpdate, this, &CRelay::handleWorkerActionUpdate);
    disconnect (&worker, &CWorker::relay1Changed, this, &CRelay::relay1Changed);
    disconnect (&worker, &CWorker::relay2Changed, this, &CRelay::relay2Changed);
    disconnect (&worker, &CWorker::relay3Changed, this, &CRelay::relay3Changed);
    disconnect (&worker, &CWorker::relay4Changed, this, &CRelay::relay4Changed);
}

//==========================================================================
// Handle worker signal - message
//==========================================================================
void CRelay::handleWorkerMessage (const QString aMsg)
{
    QString str = aMsg;

    DEBUG_PRINTF ("Worker message: %s", aMsg.toUtf8().data());
    str.append ("<br>");
    emit message (str);
}

//==========================================================================
// Handle worker signal - actionUpdate
//==========================================================================
void CRelay::handleWorkerActionUpdate (int aAction, int aResult, const QString aMsg)
{
    DEBUG_PRINTF ("Worker ACTION=%d, RESULT=%d", aAction, aResult);

    switch (aAction)
    {
    case ACTION_TEST_PORT:
        emit testDone (aResult, aMsg);
        break;
    case ACTION_CONNECT:
        if (aResult == ACTION_RESULT_OK)
            emit connected (worker.currentRelayId);
        break;
    case ACTION_DISCONNECT:
        emit disconnected ();
        break;
    }
}


//==========================================================================
// Stop all
//==========================================================================
void CRelay::stop (void)
{
    worker.requestStopAction();
    worker.requestAction(ACTION_DISCONNECT);
}

//==========================================================================
// Test Port
//==========================================================================
int CRelay::testPort (QString aPortName)
{
    DEBUG_PRINTF ("testPort %s.", aPortName.toUtf8().data());
    if (worker.setPortname(aPortName) < 0)
    {
        emit message (tr ("Unable to set port name."));
        return -1;
    }
    if (worker.requestAction(ACTION_TEST_PORT) < 0)
    {
        emit message (tr ("Fail to set action."));
        return -1;
    }
    return 0;
}

//==========================================================================
// Open port
//==========================================================================
int CRelay::openPort (QString aPortName)
{
    DEBUG_PRINTF ("connect %s.", aPortName.toUtf8().data());
    if (worker.setPortname(aPortName) < 0)
    {
        emit message (tr ("Unable to set port name."));
        return -1;
    }
    if (worker.requestAction(ACTION_CONNECT) < 0)
    {
        emit message (tr ("Fail to set action."));
        return -1;
    }
    return 0;
}

//==========================================================================
// Set Relay
//==========================================================================
int CRelay::setRelay (int aIdx, int aValue)
{
    if (worker.requestChangeRelay (aIdx, aValue) < 0)
    {
        emit message (tr ("Unable to set relay."));
        return -1;
    }

    return 0;
}

//==========================================================================
//==========================================================================
// Public members
// ^^^^^^^^^^^^^^^
//==========================================================================
//==========================================================================
// Private members
// vvvvvvvvvvvvvv
//==========================================================================
//==========================================================================
