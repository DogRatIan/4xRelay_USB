import QtQuick 2.9
import QtQuick.Controls 2.2
import "dogratian"

Page {
    id: page
    //    property alias message: labelMessage.text
    property alias listPort: listPort
    property alias buttonTest: buttonTest
    property alias buttonConnect: buttonConnect
    property alias buttonDisconnect: buttonDisconnect
    property alias buttonReload: buttonReload
    property alias message: textMessage.text
    property alias buttonClose: buttonClose

    title: qsTr("Setting")

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#444444"
            }

            GradientStop {
                position: 0.5
                color: "#ffffff"
            }
            GradientStop {
                position: 1
                color: "#444444"
            }
        }
    }

    Item {
        width: 480
        height: 480
        anchors.horizontalCenter: parent.horizontalCenter

        Label {
            x: 30
            y: 30
            width: 50
            text: "PORT:"
            font.bold: true
            font.pointSize: 12
            horizontalAlignment: Text.AlignRight
        }

        Rectangle {
            x: 90
            y: 30
            width: 380
            height: 100

            ListView {
                id: listPort
                anchors.fill: parent
                clip: true
                delegate: Item {
                    width: listPort.width
                    height: 30
                    Row {
                        Text {
                            text: name
                            padding: 5
                            font.pointSize: 12
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            text: desc
                            padding: 5
                            font.pointSize: 12
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: listPort.currentIndex = index
                    }
                }
                highlight: Rectangle {
                    color: 'grey'
                    Text {
                        anchors.centerIn: parent
                    }
                }
            }

            Rectangle {
                color: listPort.enabled ? "#00888888" : "#88888888"
                border.width: 2
                anchors.fill: parent
                border.color: "#8888ff"
            }
        }

        Item {
            id: rowButton
            x: 90
            y: 140
            width: 380
            height: 50

            Button {
                id: buttonTest
                text: qsTr("Test")
                anchors.left: parent.left
                anchors.leftMargin: 0
                width: parent.width * 0.3
            }

            Button {
                id: buttonConnect
                text: qsTr("Connect")
                enabled: true
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.3
            }

            Button {
                id: buttonDisconnect
                text: qsTr("Disconnect")
                enabled: false
                anchors.right: parent.right
                anchors.rightMargin: 0
                width: parent.width * 0.3
            }
        }

        ImageButton {
            id: buttonReload
            x: 469
            y: 85
            text: qsTr("Reload")
            icon: "../asset/refresh.png"
            iconSize: 32
        }

        Text {
            id: textMessage
            x: 90
            y: 255
            width: 380
            height: 200
            text: qsTr("Message")
            font.pointSize: 12
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
        }

        Button {
            id: buttonClose
            x: 358
            y: 195
            width: 114
            text: qsTr("Back")
        }
    }
}
