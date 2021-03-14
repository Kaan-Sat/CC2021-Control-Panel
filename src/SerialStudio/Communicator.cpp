/*
 * Copyright (c) 2021 Alex Spataru <https://github.com/alex-spataru>
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

#include "Communicator.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QFileDialog>
#include <QJsonObject>
#include <QHostAddress>
#include <QJsonDocument>

#include <qtcsv/reader.h>
#include <Misc/Utilities.h>
#include <Misc/TimerEvents.h>

using namespace SerialStudio;

#define SERIAL_STUDIO_PLUGINS_PORT 7777
static Communicator *INSTANCE = nullptr;

Communicator::Communicator()
{
    // Set default values
    m_row = 0;
    m_currentTime = "";
    m_currentSimulationData = "";
    m_simulationEnabled = false;
    m_simulationActivated = false;
    m_payload1TelemetryEnabled = true;
    m_payload2TelemetryEnabled = true;
    m_containerTelemetryEnabled = true;

    // Connect socket signals/slots
    connect(&m_socket, &QTcpSocket::connected, this, &Communicator::connectedChanged);
    connect(&m_socket, &QTcpSocket::disconnected, this, &Communicator::connectedChanged);

    // Timer module signals/slots
    auto te = Misc::TimerEvents::getInstance();
    connect(te, &Misc::TimerEvents::timeout5Hz, this, &Communicator::tryConnection);
    connect(te, &Misc::TimerEvents::timeout1Hz, this, &Communicator::sendSimulatedData);
    connect(te, &Misc::TimerEvents::timeout42Hz, this, &Communicator::updateCurrentTime);
}

Communicator *Communicator::getInstance()
{
    if (!INSTANCE)
        INSTANCE = new Communicator;

    return INSTANCE;
}

bool Communicator::connectedToSerialStudio() const
{
    return m_socket.isOpen() && m_socket.isWritable();
}

bool Communicator::simulationEnabled() const
{
    return m_simulationEnabled;
}

bool Communicator::simulationActivated() const
{
    return simulationEnabled() && m_simulationActivated;
}

bool Communicator::payload1TelemetryEnabled() const
{
    return m_payload1TelemetryEnabled;
}

bool Communicator::payload2TelemetryEnabled() const
{
    return m_payload2TelemetryEnabled;
}

bool Communicator::containerTelemetryEnabled() const
{
    return m_containerTelemetryEnabled;
}

QString Communicator::currentTime() const
{
    return m_currentTime;
}

QString Communicator::csvFileName() const
{
    if (m_csvFile.isOpen())
    {
        auto fileInfo = QFileInfo(m_csvFile.fileName());
        return fileInfo.fileName();
    }

    return tr("No CSV file selected");
}

QString Communicator::currentSimulatedReading() const
{
    return m_currentSimulationData;
}

void Communicator::openCsv()
{
    // clang-format off
    auto name = QFileDialog::getOpenFileName(Q_NULLPTR,
                                             tr("Select CSV file"),
                                             QDir::homePath(),
                                             tr("CSV files") + " (*.csv)");
    // clang-format on

    // User did not select a file, abort
    if (name.isEmpty())
        return;

    // Close current file
    if (m_csvFile.isOpen())
        m_csvFile.close();

    // Open the selected file
    m_csvFile.setFileName(name);
    if (m_csvFile.open(QFile::ReadOnly))
    {
        // Disable simulation mode
        if (simulationActivated())
            setSimulationActivated(false);

        // Read CSV data
        m_row = 0;
        m_currentSimulationData = "";
        m_csvData = QtCSV::Reader::readToList(m_csvFile);
        emit currentSimulatedReadingChanged();
    }

    // Open failure, alert user through a messagebox
    else
        Misc::Utilities::showMessageBox(tr("File open error"), m_csvFile.errorString());

    // Update UI
    emit csvFileNameChanged();
}

void Communicator::tryConnection()
{
    if (!m_socket.isOpen())
        m_socket.connectToHost(QHostAddress::LocalHost, SERIAL_STUDIO_PLUGINS_PORT);
}

void Communicator::releasePayload1()
{
    if (connectedToSerialStudio())
        sendData("CMD,1714,SP,R1;");
}

void Communicator::releasePayload2()
{
    if (connectedToSerialStudio())
        sendData("CMD,1714,SP,R2;");
}

void Communicator::updateContainerTime()
{
    if (connectedToSerialStudio())
    {
        m_currentTime = QDateTime::currentDateTime().toString("hh:mm:ss");
        emit currentTimeChanged();

        sendData("CMD,1714,ST," + m_currentTime + ";");
    }
}

void Communicator::setSimulationMode(const bool enabled)
{
    if (connectedToSerialStudio())
    {
        m_simulationActivated = false;
        m_simulationEnabled = enabled;
        emit simulationEnabledChanged();
        emit simulationActivatedChanged();

        QString cmd = "DISABLE";
        if (simulationEnabled())
            cmd = "ENABLE";

        sendData("CMD,1714,SIM," + cmd + ";");
    }
}

void Communicator::setSimulationActivated(const bool activated)
{
    if (connectedToSerialStudio() && simulationEnabled())
    {
        if (activated)
        {
            m_simulationActivated = true;
            emit simulationActivatedChanged();
            sendData("CMD,1714,SIM,ACTIVATE;");
        }

        else
            setSimulationMode(false);
    }
}

void Communicator::setPayload1TelemetryEnabled(const bool enabled)
{
    if (connectedToSerialStudio())
    {
        m_payload1TelemetryEnabled = enabled;
        emit payload1TelemetryEnabledChanged();

        QString cmd = "OFF";
        if (enabled)
            cmd = "ON";

        sendData("CMD,1714,SP1X," + cmd + ";");
    }
}

void Communicator::setPayload2TelemetryEnabled(const bool enabled)
{
    if (connectedToSerialStudio())
    {
        m_payload2TelemetryEnabled = enabled;
        emit payload2TelemetryEnabledChanged();

        QString cmd = "OFF";
        if (enabled)
            cmd = "ON";

        sendData("CMD,1714,SP2X," + cmd + ";");
    }
}

void Communicator::setContainerTelemetryEnabled(const bool enabled)
{
    if (connectedToSerialStudio())
    {
        m_containerTelemetryEnabled = enabled;
        emit containerTelemetryEnabledChanged();

        QString cmd = "OFF";
        if (enabled)
            cmd = "ON";

        sendData("CMD,1714,CX," + cmd + ";");
    }
}

void Communicator::updateCurrentTime()
{
    m_currentTime = QDateTime::currentDateTime().toString("hh:mm:ss");
    emit currentTimeChanged();
}

void Communicator::sendSimulatedData()
{
    // Stop if simulation mode is not active
    if (!simulationActivated() || !connectedToSerialStudio())
        return;

    // Read, validate & send current row data
    if (m_row < m_csvData.count() && m_row >= 0)
    {
        // Get CSV row data
        auto row = m_csvData.at(m_row);

        // Validate column count
        if (row.count() == 1)
        {
            // Get column data
            m_currentSimulationData = row.first();
            emit currentSimulatedReadingChanged();

            // Check if data is a number & send
            bool ok = false;
            auto num = m_currentSimulationData.toDouble(&ok);
            if (ok)
            {
                auto numStr = QString::number(num);
                sendData("CMD,1714,SIMP," + numStr + ";");
            }
        }

        // Column count invalid
        else
            Misc::Utilities::showMessageBox(
                tr("Simulation CSV error"),
                tr("Invalid column count at row %1").arg(m_row));

        // Increment row
        ++m_row;
    }

    // Show CSV finished box & disable simulation mode
    else
    {
        Misc::Utilities::showMessageBox(tr("Pressure simulation finished"),
                                        tr("Reached end of CSV file"));
        setSimulationActivated(false);
    }
}

void Communicator::onErrorOccurred(const QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    Misc::Utilities::showMessageBox(tr("TCP socket error"), m_socket.errorString());
}

bool Communicator::sendData(const QString &data)
{
    if (m_socket.isWritable() && m_socket.isOpen() && !data.isEmpty())
    {
        auto bytes = QString(data + "\n").toUtf8();
        auto wleng = m_socket.write(bytes);

        auto sentData = bytes;
        sentData.chop(bytes.length() - wleng);

        emit rx("TX: " + QString::fromUtf8(sentData));
        return sentData.length() == bytes.length();
    }

    return false;
}
