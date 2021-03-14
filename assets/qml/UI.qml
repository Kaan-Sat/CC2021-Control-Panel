/*
 * Copyright (c) 2020-2021 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import QtQuick 2.12
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12

Page {
    id: root

    //
    // Background
    //
    background: Image {
        opacity: 0.72
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/images/background.jpg"
    }

    //
    // Toolbar
    //
    RowLayout {
        id: toolbar

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: app.spacing
        }

        ColumnLayout {
            spacing: app.spacing
            Layout.alignment: Qt.AlignVCenter

            Image {
                opacity: 0.8
                sourceSize.width: 240
                source: "qrc:/images/kaansat.svg"
                Layout.alignment: Qt.AlignVCenter
            }

            Label {
                opacity: 0.6
                font.pixelSize: 10
                text: Cpp_AppName + " v" + Cpp_AppVersion
            }
        }

        Item {
            Layout.fillWidth: true
        }

        Label {
            font.bold: true
            font.pixelSize: 18
            font.family: app.monoFont
            Layout.alignment: Qt.AlignVCenter
            text: Cpp_SerialStudio_Communicator.currentTime
        }

        Item {
            Layout.fillWidth: true
        }

        Label {
            Layout.alignment: Qt.AlignVCenter
            text: qsTr("Serial Studio Connection")
        }

        Switch {
            Layout.alignment: Qt.AlignVCenter
            Universal.accent: Universal.Green
            checked: Cpp_SerialStudio_Communicator.connectedToSerialStudio

            MouseArea {
                anchors.fill: parent
            }
        }
    }

    //
    // User interface
    //
    RowLayout {
        anchors.fill: parent
        spacing: app.spacing
        anchors.margins: 2 * app.spacing
        anchors.topMargin: 3 * app.spacing + toolbar.height

        //
        // Buttons
        //
        GridLayout {
            id: grid
            columns: 2
            rowSpacing: app.spacing
            columnSpacing: app.spacing
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: app.width * 3/5
            Layout.maximumWidth: app.width * 3/5

            property real columnHeight: 72
            property real columnWidth: app.width * 3/10 - grid.columnSpacing

            Button {
                id: simModeEnabled
                text: qsTr("Simulation mode")

                checked: Cpp_SerialStudio_Communicator.simulationEnabled
                enabled: Cpp_SerialStudio_Communicator.connectedToSerialStudio
                onClicked: Cpp_SerialStudio_Communicator.simulationEnabled = !Cpp_SerialStudio_Communicator.simulationEnabled

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: "qrc:/icons/simulation.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: activateSimMode
                text: qsTr("Activate simulation mode")

                enabled: simModeEnabled.checked && simModeEnabled.enabled
                checked: Cpp_SerialStudio_Communicator.simulationActivated
                onClicked: Cpp_SerialStudio_Communicator.simulationActivated = !Cpp_SerialStudio_Communicator.simulationActivated

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: "qrc:/icons/start.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: telemetryEnabled

                text: qsTr("Container telemetry")

                enabled: Cpp_SerialStudio_Communicator.connectedToSerialStudio
                checked: Cpp_SerialStudio_Communicator.containerTelemetryEnabled
                onClicked: Cpp_SerialStudio_Communicator.containerTelemetryEnabled = !Cpp_SerialStudio_Communicator.containerTelemetryEnabled

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: checked ? "qrc:/icons/telemetry-on.svg" :
                                       "qrc:/icons/telemetry-off.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: updateTime
                text: qsTr("Update container time")

                onClicked: Cpp_SerialStudio_Communicator.updateContainerTime()
                enabled: Cpp_SerialStudio_Communicator.connectedToSerialStudio

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: "qrc:/icons/time.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: payload1Telemetry

                checkable: true
                text: qsTr("SP1X telemetry")

                enabled: Cpp_SerialStudio_Communicator.connectedToSerialStudio
                checked: Cpp_SerialStudio_Communicator.payload1TelemetryEnabled
                onClicked: Cpp_SerialStudio_Communicator.payload1TelemetryEnabled = !Cpp_SerialStudio_Communicator.payload1TelemetryEnabled

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: checked ? "qrc:/icons/telemetry-on.svg" :
                                       "qrc:/icons/telemetry-off.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: releasePayload1
                text: qsTr("Release SPX1")

                onClicked: Cpp_SerialStudio_Communicator.releasePayload1()
                enabled: Cpp_SerialStudio_Communicator.connectedToSerialStudio

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: "qrc:/icons/filter-1.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: payload2Telemetry

                checkable: true
                text: qsTr("SP2X telemetry")

                enabled: Cpp_SerialStudio_Communicator.connectedToSerialStudio
                checked: Cpp_SerialStudio_Communicator.payload2TelemetryEnabled
                onClicked: Cpp_SerialStudio_Communicator.payload2TelemetryEnabled = !Cpp_SerialStudio_Communicator.payload2TelemetryEnabled

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: checked ? "qrc:/icons/telemetry-on.svg" :
                                       "qrc:/icons/telemetry-off.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: releasePayload2
                text: qsTr("Release SPX2")

                onClicked: Cpp_SerialStudio_Communicator.releasePayload2()
                enabled: Cpp_SerialStudio_Communicator.connectedToSerialStudio

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: "qrc:/icons/filter-2.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Item {
                Layout.fillHeight: true
            }

            Item {
                Layout.fillHeight: true
            }

            Button {
                text: qsTr("Open simulation CSV")

                icon.width: 24
                icon.height: 24
                icon.source: "qrc:/icons/cog.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight / 2
                Layout.maximumHeight: grid.columnHeight / 2

                onClicked: Cpp_SerialStudio_Communicator.openCsv()
            }

            Label {
                verticalAlignment: Label.AlignVCenter
                horizontalAlignment: Label.AlignHCenter
                text: "<" + Cpp_SerialStudio_Communicator.csvFileName + ">"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight / 2
                Layout.maximumHeight: grid.columnHeight / 2
            }
        }

        //
        // Console display
        //
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                spacing: app.spacing
                Layout.fillWidth: true

                Label {
                    text: qsTr("CSV Row") + ":"
                    Layout.alignment: Qt.AlignVCenter
                }

                TextField {
                    color: "#72d5a3"
                    readOnly: true
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    font.family: app.monoFont
                    Layout.alignment: Qt.AlignVCenter
                    placeholderText: qsTr("No CSV data loaded")

                    text: Cpp_SerialStudio_Communicator.currentSimulatedReading

                    background: Rectangle {
                        border.width: 1
                        color: "#aa000000"
                        border.color: "#bebebe"
                    }

                    MouseArea {
                        anchors.fill: parent
                    }
                }
            }

            ScrollView {
                id: scrollView
                clip: true
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentWidth: parent.width
                ScrollBar.horizontal.policy: ScrollBar.AsNeeded

                TextArea {
                    id: textArea
                    readOnly: true
                    color: "#72d5a3"
                    font.pixelSize: 12

                    font.family: app.monoFont
                    textFormat: Text.PlainText
                    width: scrollView.contentWidth
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    placeholderText: qsTr("No data received so far") + "..."

                    Connections {
                        target: Cpp_SerialStudio_Communicator
                        function onRx(data) {
                            textArea.text += data
                        }
                    }

                    background: Rectangle {
                        border.width: 1
                        color: "#aa000000"
                        border.color: "#bebebe"
                    }

                    onTextChanged: {
                        if (scrollView.contentHeight > scrollView.height)
                            textArea.cursorPosition = textArea.length - 1
                    }
                }
            }
        }
    }
}
