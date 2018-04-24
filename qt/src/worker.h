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
#ifndef _INC_WORKER_H
#define _INC_WORKER_H

//==========================================================================
//==========================================================================
#include <QThread>
#include <QMutex>
#include <QSerialPort>
#include <QElapsedTimer>

//==========================================================================
// Defines
//==========================================================================
enum
{
    ACTION_NONE = 0,
    ACTION_TEST_PORT,
    ACTION_CONNECT,
    ACTION_DISCONNECT,
    ACTION_SET_RELAY
};

#define ACTION_RESULT_OK            0
#define ACTION_RESULT_FAIL          -1
#define ACTION_REAULT_IN_PROGRESS   1

#define NUM_OF_RELAY            4
#define VALUE_REALY_UNKNOWN     -1
#define VALUE_RELAY_ON          1
#define VALUE_RELAY_OFF         0

// ms
#define TIMEOUT_RELAY_COMMAND_DEFAULT   500

//==========================================================================
// Class define
//==========================================================================
class CWorker : public QThread
{
    Q_OBJECT

public:
    CWorker (QObject * aParent = 0);
    ~CWorker ();

    void run() Q_DECL_OVERRIDE;

    void requestAbort (void);
    int setPortname (QString aPortName);
    int requestAction (int aAction);
    void requestStopAction (void);
    int requestChangeRelay (int aIdx, int aValue);

    QString currentRelayId;


signals:
    void message (const QString aMsg);
    void actionUpdate (int aAction, int aResult, const QString aMsg);
    void relay1Changed (int aStatus);
    void relay2Changed (int aStatus);
    void relay3Changed (int aStatus);
    void relay4Changed (int aStatus);

private:
    QMutex mutex;
    QSerialPort *serialPort;

    bool abort;
    QString errorMessage;

    bool stopAction;
    int currentAction;
    QString currentPortname;
    int requestRelayIdx;
    int requestRelayValue;

    bool relayConnected;
    QElapsedTimer timerUpdateStatus;
    int relayStatus[NUM_OF_RELAY];

    int openPort (QString aPortName);
    void closePort (void);
    int sendCommand (const char *aCmd, char *aResp, int aRespLen, unsigned long aTimeout = TIMEOUT_RELAY_COMMAND_DEFAULT);

    int getRelayStatus (void);

    void doTestPort (void);
    void doConnect (void);
    void doSetRelay (void);

    void processConnected (void);

};

//==========================================================================
//==========================================================================
#endif

