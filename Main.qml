import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: root
    visible: true
    width: 840
    height: 600
    title: qsTr("Valve tuner")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("Port")
            }
            ComboBox {
                id: portCombo
                Layout.fillWidth: true
                editable: true

                // список доступных COM-портов из C++
                model: controller.availablePorts

                // выбор из списка
                onCurrentTextChanged: controller.portName = currentText
                // ручной ввод
                onEditTextChanged: controller.portName = editText

            }
            Button {
                text: controller.connected ? qsTr("Disconnect") : qsTr("Connect")
                onClicked: controller.connectOrDisconnect()
            }
        }

        Button {
            Layout.alignment: Qt.AlignLeft
            text: controller.running ? qsTr("Stop") : qsTr("Start")
            enabled: controller.connected
            onClicked: controller.startOrStop()
        }

        RowLayout {
            Layout.fillWidth: true

            Label { text: qsTr("PWM") }
            Label {
                Layout.minimumWidth: 80
                text: controller.pwm
            }

            Label { text: qsTr("FLOW") }
            Label {
                Layout.minimumWidth: 80
                text: Number(controller.flow).toFixed(3)
            }

            Label { text: qsTr("error") }
            Label {
                Layout.minimumWidth: 80
                text: Number(controller.error).toFixed(3)
            }
        }

        RowLayout {
            Layout.fillWidth: true

            ColumnLayout {
                Label { text: qsTr("PWM 1") }
                Label { text: qsTr("FLOW 1") }
            }

            ColumnLayout {
                Label { text: controller.pwm1 }
                Label { text: Number(controller.flow1).toFixed(3) }
            }

            ColumnLayout {
                Label { text: qsTr("PWM 2") }
                Label { text: qsTr("FLOW 2") }
            }

            ColumnLayout {
                Label { text: controller.pwm2 }
                Label { text: Number(controller.flow2).toFixed(3) }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Result approximation")
                font.bold: true
            }

            RowLayout {
                Layout.fillWidth: true

                Label { text: qsTr("Slope") }
                Label {
                    Layout.minimumWidth: 100
                    text: controller.slope ? Number(controller.slope).toFixed(2) : "-"
                }

                Label { text: qsTr("Offset") }
                Label {
                    Layout.minimumWidth: 100
                    text: controller.offset ? controller.offset : "-"
                }
            }
        }

        Label { text: qsTr("Log") }
        TextArea {
            Layout.fillWidth: true
            Layout.fillHeight: true
            readOnly: true
            text: controller.logText
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
