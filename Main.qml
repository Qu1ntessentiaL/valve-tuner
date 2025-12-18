import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: root
    visible: true
    width: 960
    height: 640
    title: qsTr("Valve tuner")
    color: "#20232a"
    font.family: "Segoe UI"
    font.pixelSize: 18

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("Port")
                color: "#ffffff"
            }
            ComboBox {
                id: portCombo
                Layout.fillWidth: true
                editable: true
                implicitHeight: 40
                font.pixelSize: 16

                // список доступных COM-портов из C++
                model: controller.availablePorts

                // выбор из списка
                onCurrentTextChanged: controller.portName = currentText
                // ручной ввод
                onEditTextChanged: controller.portName = editText

            }
            Button {
                text: controller.connected ? qsTr("Disconnect") : qsTr("Connect")
                Layout.preferredWidth: 150
                implicitHeight: 40
                font.pixelSize: 16
                onClicked: controller.connectOrDisconnect()
            }
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            text: controller.running ? qsTr("Stop") : qsTr("Start")
            enabled: controller.connected
            Layout.preferredWidth: 200
            implicitHeight: 48
            font.pixelSize: 20
            onClicked: controller.startOrStop()
        }

        Frame {
            Layout.fillWidth: true
            Layout.preferredHeight: 140
            background: Rectangle {
                color: "#282c34"
                radius: 8
            }

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 40

                ColumnLayout {
                    spacing: 4
                    Label { text: qsTr("PWM"); color: "#bbbbbb" }
                    Label {
                        Layout.minimumWidth: 80
                        text: controller.pwm
                        color: "#ffffff"
                        font.bold: true
                    }
                }

                ColumnLayout {
                    spacing: 4
                    Label { text: qsTr("FLOW"); color: "#bbbbbb" }
                    Label {
                        Layout.minimumWidth: 80
                        text: Number(controller.flow).toFixed(3)
                        color: "#ffffff"
                        font.bold: true
                    }
                }

                ColumnLayout {
                    spacing: 4
                    Label { text: qsTr("Error"); color: "#bbbbbb" }
                    Label {
                        Layout.minimumWidth: 80
                        text: Number(controller.error).toFixed(3)
                        color: "#ffffff"
                        font.bold: true
                    }
                }

                Item { Layout.fillWidth: true }

                ColumnLayout {
                    spacing: 4
                    Label { text: qsTr("PWM 1 / FLOW 1"); color: "#bbbbbb" }
                    Label {
                        Layout.minimumWidth: 120
                        text: qsTr("%1 / %2").arg(controller.pwm1).arg(Number(controller.flow1).toFixed(3))
                        color: "#ffffff"
                    }
                }

                ColumnLayout {
                    spacing: 4
                    Label { text: qsTr("PWM 2 / FLOW 2"); color: "#bbbbbb" }
                    Label {
                        Layout.minimumWidth: 120
                        text: qsTr("%1 / %2").arg(controller.pwm2).arg(Number(controller.flow2).toFixed(3))
                        color: "#ffffff"
                    }
                }
            }
        }

        Frame {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            background: Rectangle {
                color: "#282c34"
                radius: 8
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Result approximation")
                    font.bold: true
                    font.pixelSize: 20
                    color: "#ffffff"
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 24

                    Label { text: qsTr("Slope"); color: "#bbbbbb" }
                    Label {
                        Layout.minimumWidth: 120
                        text: controller.slope ? Number(controller.slope).toFixed(2) : "-"
                        color: "#ffffff"
                        font.bold: true
                    }

                    Label { text: qsTr("Offset"); color: "#bbbbbb" }
                    Label {
                        Layout.minimumWidth: 120
                        text: controller.offset ? controller.offset : "-"
                        color: "#ffffff"
                        font.bold: true
                    }

                    Item { Layout.fillWidth: true }
                }
            }
        }

        Label {
            text: qsTr("Log")
            color: "#ffffff"
        }
        TextArea {
            Layout.fillWidth: true
            Layout.fillHeight: true
            readOnly: true
            text: controller.logText
            font.pixelSize: 16
            color: "#e0e0e0"
            wrapMode: TextArea.NoWrap
        }
    }

    Connections {
        target: controller
        function onErrorOccurred(message) {
            // Простое уведомление об ошибке
            console.warn(message)
        }
    }
}
