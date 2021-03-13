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
            Layout.alignment: Qt.AlignVCenter
            text: qsTr("Serial Studio Connection")
        }

        Switch {
            Layout.alignment: Qt.AlignVCenter
            Universal.accent: Universal.Green
        }
    }

    //
    // User interface
    //
    RowLayout {
        anchors.fill: parent
        spacing: app.spacing
        anchors.margins: app.spacing
        anchors.topMargin: 2 * app.spacing + toolbar.height

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


                checkable: true
                text: qsTr("Simulation mode")

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

                checkable: true
                enabled: simModeEnabled.checked
                text: qsTr("Activate simulation mode")

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

                checkable: true
                text: qsTr("Container telemetry")

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: "qrc:/icons/telemetry-on.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: payloadTelemetry

                checkable: true
                text: qsTr("Payload telemetry")

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: "qrc:/icons/radar.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: releasePayload1
                text: qsTr("Release payload 1")

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
                id: releasePayload2
                text: qsTr("Release payload 2")

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: "qrc:/icons/filter-2.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Button {
                id: updateTime
                text: qsTr("Update container time")

                icon.width: 42
                icon.height: 42
                font.pixelSize: 16
                icon.source: "qrc:/icons/time.svg"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight
                Layout.maximumHeight: grid.columnHeight
            }

            Item {
                Layout.fillWidth: true
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
            }

            Label {
                verticalAlignment: Label.AlignVCenter
                horizontalAlignment: Label.AlignHCenter
                text: "<" + qsTr("No CSV file selected") + ">"

                Layout.minimumWidth: grid.columnWidth
                Layout.maximumWidth: grid.columnWidth
                Layout.minimumHeight: grid.columnHeight / 2
                Layout.maximumHeight: grid.columnHeight / 2
            }
        }

        //
        // Console display
        //
        TextArea {
            opacity: 0.8
            readOnly: true
            color: "#72d5a3"
            font.pixelSize: 12
            Layout.fillWidth: true
            Layout.fillHeight: true
            font.family: app.monoFont
            textFormat: Text.PlainText
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            placeholderText: qsTr("No data received so far") + "..."

            background: Rectangle {
                color: "#000000"
                border.width: 2
                border.color: "#bebebe"
            }

            MouseArea {
                anchors.fill: parent
            }
        }
    }
}
