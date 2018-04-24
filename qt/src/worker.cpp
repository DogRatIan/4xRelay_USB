//==========================================================================
// Worker
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
#include <QString>
#include <QElapsedTimer>

#include <time.h>

#include "debug.h"

#include "worker.h"

//==========================================================================
// Defines
//==========================================================================

//==========================================================================
// Variables
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
CWorker::CWorker (QObject * aParent) :
    QThread (aParent),
    serialPort (NULL),
    abort (false),
    currentAction (ACTION_NONE),
    relayConnected(false)
{
}

//==========================================================================
// Destroyer
//==========================================================================
CWorker::~CWorker ()
{
    // Delete class
    if (serialPort != NULL)
        delete serialPort;
    serialPort = NULL;
}

//==========================================================================
// Main operation
//==========================================================================
void CWorker::run()
{
    //
    serialPort = new QSerialPort;
    closePort ();

    //
    abort = false;
    try
    {
        DEBUG_PRINTF ("Enter Worker loop...");
        while (!abort)
        {
            mutex.lock ();
            switch (currentAction)
            {
            case ACTION_NONE:
                if (relayConnected)
                {
                    processConnected();
                }
                break;

            case ACTION_TEST_PORT:
                if (relayConnected)
                {
                    emit actionUpdate (currentAction, ACTION_RESULT_FAIL, "Relay already connected.");
                    currentAction = ACTION_NONE;
                    break;
                }
                doTestPort ();
                break;

            case ACTION_CONNECT:
                if (relayConnected)
                {
                    emit actionUpdate (currentAction, ACTION_RESULT_FAIL, "Relay already connected.");
                    currentAction = ACTION_NONE;
                    break;
                }
                doConnect();
                break;

            case ACTION_DISCONNECT:
                closePort ();
                emit actionUpdate (currentAction, ACTION_RESULT_OK, "Relay disconnected.");
                currentAction = ACTION_NONE;
                break;

            case ACTION_SET_RELAY:
                doSetRelay ();
                break;

            default:
                if (currentAction != ACTION_NONE)
                {
                    emit actionUpdate (currentAction, ACTION_RESULT_FAIL, "Unknown action.");
                }
                currentAction = ACTION_NONE;
                break;
            }
            mutex.unlock ();

            usleep (10000);
        }
    }
    catch (const char *aMsg)
    {
        emit message (QString (aMsg));
    }
    catch (QString aMsg)
    {
        emit message (aMsg);
    }

    // Close port
    DEBUG_PRINTF ("Worker ended.");

    // Delete class
    if (serialPort != NULL)
    {
        serialPort->close();
        delete serialPort;
    }
    serialPort = NULL;
}

//==========================================================================
// Request Abort
//==========================================================================
void CWorker::requestAbort (void)
{
    abort = true;
}

//==========================================================================
// Set Target port
//==========================================================================
int CWorker::setPortname (QString aPortName)
{
    int curr_action;

    // Check no action running
    mutex.lock ();
    curr_action = currentAction;
    mutex.unlock ();
    if (curr_action != ACTION_NONE)
        return -1;

    // Save port name
    currentPortname = aPortName;
    return 0;
}

//==========================================================================
// Request an Action
//==========================================================================
int CWorker::requestAction (int aAction)
{
    int curr_action;

    // Check no action running
    mutex.lock ();
    curr_action = currentAction;
    mutex.unlock ();
    if (curr_action != ACTION_NONE)
        return -1;

    //
    mutex.lock ();
    stopAction = false;
    currentAction = aAction;
    mutex.unlock ();
    return 0;
}

//==========================================================================
// Stop any running action
//==========================================================================
void CWorker::requestStopAction (void)
{
    int curr_action;

    stopAction = true;
    QElapsedTimer timer;

    timer.start();
    while (timer.elapsed() < 1000)
    {
        mutex.lock ();
        curr_action = currentAction;
        mutex.unlock ();
        if (curr_action == ACTION_NONE)
            break;
    }
}

//==========================================================================
// Request to change relay status
//==========================================================================
int CWorker::requestChangeRelay (int aIdx, int aValue)
{
    int curr_action;

    // Check no action running
    mutex.lock ();
    curr_action = currentAction;
    mutex.unlock ();
    if (curr_action != ACTION_NONE)
        return -1;

    //
    mutex.lock ();
    stopAction = false;
    currentAction = ACTION_SET_RELAY;
    requestRelayIdx = aIdx;
    requestRelayValue = aValue;
    mutex.unlock ();
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

//==========================================================================
// Open Port
//==========================================================================
int CWorker::openPort (QString aPortName)
{
    // Check class
    if (serialPort == NULL)
    {
        errorMessage = tr ("QSerialPort class not created.");
        return -1;
    }

    // Unconditional close
    serialPort->close ();

    // Open and setup port
    serialPort->setPortName (aPortName);
    if (!serialPort->open (QIODevice::ReadWrite))
    {
        errorMessage = QString ("%1 ERROR: %2").arg(aPortName).arg(serialPort->errorString ());
        return -1;
    }

    if (!serialPort->setBaudRate (QSerialPort::Baud115200))
    {
        errorMessage = QString ("%1 ERROR: %2").arg(aPortName).arg(serialPort->errorString ());
        return -1;
    }

    if (!serialPort->setDataBits (QSerialPort::Data8))
    {
        errorMessage = QString ("%1 ERROR: %2").arg(aPortName).arg(serialPort->errorString ());
        return -1;
    }

    if (!serialPort->setParity (QSerialPort::NoParity))
    {
        errorMessage = QString ("%1 ERROR: %2").arg(aPortName).arg(serialPort->errorString ());
        return -1;
    }

    if (!serialPort->setStopBits (QSerialPort::OneStop))
    {
        errorMessage = QString ("%1 ERROR: %2").arg(aPortName).arg(serialPort->errorString ());
        return -1;
    }

    if (!serialPort->setFlowControl(QSerialPort::NoFlowControl))
    {
        errorMessage = QString ("%1 ERROR: %2").arg(aPortName).arg(serialPort->errorString ());
        return -1;
    }

    serialPort->flush();

    return 0;
}

//==========================================================================
// Close Port
//==========================================================================
void CWorker::closePort (void)
{
    int i;

    // Check class
    if (serialPort == NULL)
    {
        errorMessage = tr ("QSerialPort class not created.");
        return;
    }

    // Close port
    serialPort->close();
    relayConnected = false;
    currentRelayId.clear();
    for (i = 0; i < NUM_OF_RELAY; i ++)
        relayStatus[i] = VALUE_REALY_UNKNOWN;
    emit relay1Changed (VALUE_REALY_UNKNOWN);
    emit relay2Changed (VALUE_REALY_UNKNOWN);
    emit relay3Changed (VALUE_REALY_UNKNOWN);
    emit relay4Changed (VALUE_REALY_UNKNOWN);
}


//==========================================================================
// Send command
//==========================================================================
int CWorker::sendCommand (const char *aCmd, char *aResp, int aRespLen, unsigned long aTimeout)
{
    QElapsedTimer timer;
    QString str;
    char buf[128];
    int buf_len;

    // Check class
    if (serialPort == NULL)
    {
        errorMessage = tr ("QSerialPort class not created.");
        return -1;
    }

    // Check port opened
    if (!serialPort->isOpen())
    {
        errorMessage = tr ("Port not opened.");
        return -1;
    }

    // Send out data
    qsnprintf(buf, sizeof (buf), "%s\n", aCmd);
    DEBUG_PRINTF ("TX> %s", buf);
    DEBUG_PRINTF ("Tx %d bytes.", qstrlen (buf));
    serialPort->write (buf, qstrlen (buf));

    // Wait response
    str.clear();
    memset (aResp, 0, aRespLen);
    timer.start();
    while (timer.elapsed() < aTimeout)
    {
        // Check aborted
        if ((abort) || (stopAction))
        {
            errorMessage = tr ("Aborted.");
            return -1;
        }

        // Check rxed
        memset (buf, 0, sizeof (buf));
        serialPort->waitForReadyRead (0);
        buf_len = serialPort->read(buf, sizeof (buf) - 1);

        // Error
        if (buf_len < 0)
        {
            errorMessage = QString ("%1 ERROR: %2").arg(currentPortname).arg(serialPort->errorString ());
            return -1;
        }

        // No data
        if (buf_len == 0)
            continue;

        //
        DEBUG_PRINTF ("Rxed %d bytes.", buf_len);
        str.append (buf);
        if (str.indexOf("\n") >= 0)
            break;
    }

    //
    str.replace ("\n", "");
    DEBUG_PRINTF ("RX> %s", str.toUtf8().data());
    qstrncpy (aResp, str.toUtf8().data(), aRespLen);

    return qstrlen (aResp);
}

//==========================================================================
// Get Relay status
//==========================================================================
int CWorker::getRelayStatus (void)
{
    int i;
    char cmd[64];
    char resp[64];
    int result [NUM_OF_RELAY];

    // Clear relay status
    for (i = 0; i < NUM_OF_RELAY; i ++)
        result[i] = VALUE_RELAY_OFF;

    // Get status
    for (i = 0; i < NUM_OF_RELAY; i ++)
    {
        qsnprintf (cmd, sizeof (cmd), "G%d", i + 1);
        if (sendCommand (cmd, resp, sizeof (resp)) < 0)
        {
            emit message (tr ("Fail to get relay status."));
            return -1;
        }
        if (strcmp (resp, "0") == 0)
        {
            result[i] = VALUE_RELAY_OFF;
        }
        else if (strcmp (resp, "1") == 0)
        {
            result[i] = VALUE_RELAY_ON;
        }
        else
        {
            emit message (tr ("getRelayStatus: Unknown response."));
            return -1;
        }
    }

    // Send signals
    if (result[0] != relayStatus[0])
        emit relay1Changed (result[0]);
    if (result[1] != relayStatus[1])
        emit relay2Changed (result[1]);
    if (result[2] != relayStatus[2])
        emit relay3Changed (result[2]);
    if (result[3] != relayStatus[3])
        emit relay4Changed (result[3]);

    // Store status
    memcpy (&relayStatus, &result, sizeof (relayStatus));

    //
    return 0;
}


//==========================================================================
// DO ACTION_TEST_PORT
//==========================================================================
void CWorker::doTestPort (void)
{
    QString str;
    QString relay_id;
    QString relay_version;
    char resp[64];

    do
    {
        // Open port
        if (openPort(currentPortname) < 0)
        {
            emit actionUpdate (currentAction, ACTION_RESULT_FAIL, errorMessage);
            break;
        }
        emit message (tr("Port opened."));

        // Send dummy command
        sendCommand ("", resp, sizeof (resp), 500);

        // Get ID
        if (sendCommand ("GI", resp, sizeof (resp)) < 0)
        {
            emit actionUpdate (currentAction, ACTION_RESULT_FAIL, errorMessage);
            break;
        }
        if (qstrcmp (resp, "4xRelay-USB") != 0)
        {
            emit actionUpdate (currentAction, ACTION_RESULT_FAIL, "4xRelay not found.");
            break;
        }
        relay_id = resp;

        // Get version
        if (sendCommand ("GV", resp, sizeof (resp)) < 0)
        {
            emit actionUpdate (currentAction, ACTION_RESULT_FAIL, errorMessage);
            break;
        }
        relay_version = resp;

        // Save relay ID
        emit message (QString ("%1 %2 found.").arg(relay_id).arg(relay_version));

        // All done
        str.sprintf ("%s port test ok.", currentPortname.toUtf8().data());
        emit actionUpdate (currentAction, ACTION_RESULT_OK, str);

    } while (0);

    //
    if (serialPort != NULL)
        serialPort->close ();
    currentAction = ACTION_NONE;
}

//==========================================================================
// DO ACTION_CONNECT
//==========================================================================
void CWorker::doConnect (void)
{
    QString str;
    QString relay_id;
    QString relay_version;
    char resp[64];

    do
    {
        // Open port
        if (openPort(currentPortname) < 0)
        {
            emit actionUpdate (currentAction, ACTION_RESULT_FAIL, errorMessage);
            break;
        }
        emit message (tr("Port opened."));

        // Send dummy command
        sendCommand ("", resp, sizeof (resp), 500);

        // Get ID
        if (sendCommand ("GI", resp, sizeof (resp)) < 0)
        {
            emit actionUpdate (currentAction, ACTION_RESULT_FAIL, errorMessage);
            break;
        }
        if (qstrcmp (resp, "4xRelay-USB") != 0)
        {
            emit actionUpdate (currentAction, ACTION_RESULT_FAIL, tr ("4xRelay not found."));
            break;
        }
        relay_id = resp;

        // Get version
        if (sendCommand ("GV", resp, sizeof (resp)) < 0)
        {
            emit actionUpdate (currentAction, ACTION_RESULT_FAIL, errorMessage);
            break;
        }
        relay_version = resp;

        // Save relay ID
        currentRelayId = QString ("%1 %2").arg(relay_id).arg(relay_version);
        emit message (QString ("%1 found.").arg(currentRelayId));

        // Get status
        if (getRelayStatus () < 0)
        {
            emit actionUpdate (currentAction, ACTION_RESULT_FAIL, tr ("Fail to get status."));
        }

        // All done
        emit actionUpdate (currentAction, ACTION_RESULT_OK, tr("Connected"));
        relayConnected = true;
        timerUpdateStatus.start();

    } while (0);

    //
    currentAction = ACTION_NONE;
}

//==========================================================================
// DO ACTION_SET_RELAY
//==========================================================================
void CWorker::doSetRelay (void)
{
    char cmd[64];
    char resp[64];

    //
    do
    {
        qsnprintf (cmd, sizeof (cmd), "S%d=%d", requestRelayIdx + 1, requestRelayValue);
        if (sendCommand (cmd, resp, sizeof (resp)) < 0)
        {
            emit message (tr ("Fail to set relay status."));
            break;
        }

        if (strcmp (resp, "OK") == 0)
        {

        }
        else
        {
            emit message (tr ("doSetRelay: Command fail."));
            break;
        }

        getRelayStatus ();
        timerUpdateStatus.restart();

    } while (0);

    //
    currentAction = ACTION_NONE;
}

//==========================================================================
// Process - relayConnected
//==========================================================================
void CWorker::processConnected (void)
{
    // Update relay status
    if (timerUpdateStatus.elapsed() >= 5000)
    {
        timerUpdateStatus.restart();

        if (getRelayStatus () < 0)
        {
            usleep (500000);
            if (getRelayStatus () < 0)
            {
                closePort();
                emit actionUpdate (ACTION_DISCONNECT, ACTION_RESULT_OK, "Relay disconnected.");
            }
        }
    }
}

