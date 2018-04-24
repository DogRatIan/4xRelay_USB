import QtQuick 2.9
import QtQuick.Controls 2.2
import com.dogratian.qml.PortList 1.0
import com.dogratian.qml.Relay 1.0

ApplicationWindow {
    id: topWindow
    visible: true
    width: 530
    height: 530
    minimumWidth: 530
    minimumHeight: 530
    flags: Qt.FramelessWindowHint
    title: qsTr("4xRelay-USB")
    font.family: "Verdana"

    // Shared variables
    property string message: "-";
    property bool running: false;
    property bool relayConnected: false;
    property string currentPortname: "-";
    property string relayId: "-";
    property color led1Color: "#ffffff";
    property color led2Color: "#ffffff";
    property color led3Color: "#ffffff";
    property color led4Color: "#ffffff";
    property bool switch1Checked: false;
    property bool switch2Checked: false;
    property bool switch3Checked: false;
    property bool switch4Checked: false;

    header: ToolBar {
        contentHeight: toolButton.implicitHeight

        ToolButton {
            id: toolButton
            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            font.pixelSize: Qt.application.font.pixelSize * 1.6
            onClicked: {
                if (stackView.depth > 1) {
                    stackView.pop();
                } else {
                    drawer.open()
                }
            }
        }

        Label {
            text: stackView.currentItem.title
            anchors.centerIn: parent
        }
    }

    Drawer {
        id: drawer
        width: topWindow.width * 0.20
        height: topWindow.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Setting")
                width: parent.width
                onClicked: {
                    drawer.close();
                    stackView.push(pageSetting);
                    portList.refresh();
                }
            }
            ItemDelegate {
                text: qsTr("Exit")
                width: parent.width
                onClicked: {
                    Qt.quit();
                }
            }
        }
    }

    StackView {
        id: stackView
        initialItem: pageHome
        anchors.fill: parent
    }


    Component {
        id: pageHome

        HomeForm {
            imageLinkUp.visible: topWindow.relayConnected
            imageLinkDown.visible: !topWindow.relayConnected
            labelPortName.visible: topWindow.relayConnected
            labelPortName.text: topWindow.currentPortname
            labelRelayId.text: topWindow.relayId
            led1.color: topWindow.led1Color
            led2.color: topWindow.led2Color
            led3.color: topWindow.led3Color
            led4.color: topWindow.led4Color
            switchRelay1.checked: topWindow.switch1Checked
            switchRelay2.checked: topWindow.switch2Checked
            switchRelay3.checked: topWindow.switch3Checked
            switchRelay4.checked: topWindow.switch4Checked
            switchRelay1.enabled: topWindow.relayConnected
            switchRelay2.enabled: topWindow.relayConnected
            switchRelay3.enabled: topWindow.relayConnected
            switchRelay4.enabled: topWindow.relayConnected
            buttonSetting.onClicked: {
                console.log ("CLICKED");
                stackView.push (pageSetting);
                portList.refresh ();
            }

            switchRelay1.onClicked: {
                console.log ("Switch 1 changed." + switchRelay1.checked);
                if (switchRelay1.checked)
                    relay.setRelay (0, 1);
                else
                    relay.setRelay (0, 0);
            }
            switchRelay2.onClicked: {
                console.log ("Switch 2 changed." + switchRelay2.checked);
                if (switchRelay2.checked)
                    relay.setRelay (1, 1);
                else
                    relay.setRelay (1, 0);
            }
            switchRelay3.onClicked: {
                console.log ("Switch 3 changed." + switchRelay3.checked);
                if (switchRelay3.checked)
                    relay.setRelay (2, 1);
                else
                    relay.setRelay (2, 0);
            }
            switchRelay4.onClicked: {
                console.log ("Switch 4 changed." + switchRelay4.checked);
                if (switchRelay4.checked)
                    relay.setRelay (3, 1);
                else
                    relay.setRelay (3, 0);
            }
        }
    }

    ListModel {
        id: modelPort
    }

    Component {
        id: pageSetting

        SettingForm {
            id: formSetting
            listPort.model: modelPort
            message: topWindow.message
            buttonTest.onClicked: {
                var curr_port = modelPort.get(listPort.currentIndex);
                topWindow.message = qsTr("Testing ") + curr_port.name + "...<br>";
                if (relay.testPort (curr_port.name) >= 0) {
                    topWindow.running = true;
                }
            }
            buttonConnect.onClicked: {
                var curr_port = modelPort.get(listPort.currentIndex);
                topWindow.message = qsTr("Opening ") + curr_port.name + "...<br>";
                if (relay.openPort (curr_port.name) >= 0) {
                    topWindow.running = true;
                    topWindow.currentPortname = curr_port.name;
                }
            }
            buttonDisconnect.onClicked: {
                relay.stop();
            }
            buttonReload.onClicked: {
                portList.refresh();
            }
            buttonClose.onClicked: {
                stackView.pop();
            }
            listPort.enabled: !topWindow.running
            buttonTest.enabled: !topWindow.running
            buttonConnect.enabled: !topWindow.running
            buttonDisconnect.enabled: topWindow.running
            buttonReload.enabled: !topWindow.running
        }
    }

    PortList {
        id: portList
        onChanged: {
            var i;

            modelPort.clear();
            for (i = 0; i < aCount; i ++) {
                var info = portList.getItemAt(i);
                modelPort.append ({"name": info.name, "desc": info.desc});
//                topWindow.message = "<span style='color:red;'>" + aCount + qsTr ("</span> port(s) found.")
            }
        }
    }

    Relay {
        id: relay

        onMessage: {
            topWindow.message += aMessage;
        }

        onTestDone: {
            topWindow.message += aMessage;
            topWindow.running = false;
        }

        onConnected: {
            topWindow.relayConnected = true;
            topWindow.relayId = aRelayId;
        }

        onDisconnected: {
            topWindow.message += qsTr ("Relay disconnected.");
            topWindow.running = false;
            topWindow.relayConnected = false;
            topWindow.relayId = " ";
        }

        onRelay1Changed: {
            if (aStatus == 1) {
                topWindow.led1Color = "#FF0000";
                topWindow.switch1Checked = true;
            }
            else if (aStatus == 0) {
                topWindow.led1Color = "#888888";
                topWindow.switch1Checked = false;
            }
            else
                topWindow.led1Color = "#ffffff";
        }

        onRelay2Changed: {
            if (aStatus == 1) {
                topWindow.led2Color = "#FF0000";
                topWindow.switch2Checked = true;
            }
            else if (aStatus == 0) {
                topWindow.led2Color = "#888888";
                topWindow.switch2Checked = false;
            }
            else
                topWindow.led2Color = "#ffffff";
        }

        onRelay3Changed: {
            if (aStatus == 1) {
                topWindow.led3Color = "#FF0000";
                topWindow.switch3Checked = true;
            }
            else if (aStatus == 0) {
                topWindow.led3Color = "#888888";
                topWindow.switch3Checked = false;
            }
            else
                topWindow.led3Color = "#ffffff";
        }

        onRelay4Changed: {
            if (aStatus == 1) {
                topWindow.led4Color = "#FF0000";
                topWindow.switch4Checked = true;
            }
            else if (aStatus == 0) {
                topWindow.led4Color = "#888888";
                topWindow.switch4Checked = false;
            }
            else
                topWindow.led4Color = "#ffffff";
        }
    }

}
