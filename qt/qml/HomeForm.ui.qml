import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "dogratian"

Page {
    id: page
    property alias switchRelay1: switchRelay1
    property alias switchRelay2: switchRelay2
    property alias switchRelay3: switchRelay3
    property alias switchRelay4: switchRelay4
    property alias buttonSetting: buttonSetting
    property alias imageLinkUp: imageLinkUp
    property alias imageLinkDown: imageLinkDown
    property alias labelPortName: labelPortName
    property alias labelRelayId: labelRelayId
    property alias led1: led1
    property alias led2: led2
    property alias led3: led3
    property alias led4: led4

    title: qsTr("Home")

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
    Rectangle {
        width: 480
        height: 480
        color: "#00000000"
        anchors.horizontalCenter: parent.horizontalCenter
        Image {
            id: image
            x: 0
            y: 0
            fillMode: Image.PreserveAspectFit
            source: "../asset/case.png"
        }

        ImageButton {
            id: buttonSetting
            x: 224
            y: 400
            anchors.horizontalCenterOffset: 227
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Config")
            icon: "../asset/usb.png"
            iconSize: 64
        }

        Image {
            id: imageLinkUp
            x: 263
            y: 390
            source: "../asset/link.png"
            visible: false
        }

        Image {
            id: imageLinkDown
            x: 263
            y: 390
            visible: false
            source: "../asset/link_broken.png"
        }

        Switch {
            id: switchRelay1
            x: 76
            y: 180
        }

        Switch {
            id: switchRelay2
            x: 161
            y: 180
        }

        Switch {
            id: switchRelay3
            x: 245
            y: 180
        }

        Switch {
            id: switchRelay4
            x: 332
            y: 180
        }

        Label {
            id: labelPortName
            x: 266
            y: 373
            text: qsTr("-")
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            id: labelRelayId
            x: 60
            y: 420
            width: 150
            height: 20
            text: qsTr("4xRelay-USB")
        }

        Rectangle {
            id: led1
            x: 104
            y: 108
            width: 20
            height: 10
            color: "#ffffff"
            border.width: 2
        }

        Rectangle {
            id: led2
            x: 188
            y: 108
            width: 20
            height: 10
            color: "#ffffff"
            border.width: 2
        }

        Rectangle {
            id: led3
            x: 273
            y: 108
            width: 20
            height: 10
            color: "#ffffff"
            border.width: 2
        }

        Rectangle {
            id: led4
            x: 356
            y: 108
            width: 20
            height: 10
            color: "#ffffff"
            border.width: 2
        }
    }
}
