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

/*
 * Set TCP port to communicate with Serial Studio
 */
#define SERIAL_STUDIO_PLUGINS_PORT 7777

/*
 * Pointer to singleton instance of class
 */
static Communicator *INSTANCE = nullptr;

/**
 * Constructor function
 */
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
    m_containerTelemetryEnabled = false;

    // Connect socket signals/slots
    connect(&m_socket, &QTcpSocket::disconnected, &m_socket, &QTcpSocket::close);
    connect(&m_socket, &QTcpSocket::connected, this, &Communicator::onConnectedChanged);
    connect(&m_socket, &QTcpSocket::disconnected, this,
            &Communicator::onConnectedChanged);

    // Timer module signals/slots
    auto te = Misc::TimerEvents::getInstance();
    connect(te, &Misc::TimerEvents::timeout5Hz, this, &Communicator::tryConnection);
    connect(te, &Misc::TimerEvents::timeout1Hz, this, &Communicator::sendSimulatedData);
    connect(te, &Misc::TimerEvents::timeout42Hz, this, &Communicator::updateCurrentTime);
}

/**
 * Returns a pointer to the only instance of the application
 */
Communicator *Communicator::getInstance()
{
    if (!INSTANCE)
        INSTANCE = new Communicator;

    return INSTANCE;
}

/**
 * Returns @c true if the application is connected to the Serial Studio TCP server
 */
bool Communicator::connectedToSerialStudio() const
{
    return m_socket.state() == QTcpSocket::ConnectedState;
}

/**
 * Returns @c true if the simulation mode is enabled
 */
bool Communicator::simulationEnabled() const
{
    return m_simulationEnabled;
}

/**
 * Returns @c true if simulation mode is enabled & active
 */
bool Communicator::simulationActivated() const
{
    return simulationEnabled() && m_simulationActivated;
}

/**
 * Returns @c true if SP1 telemetry is enabled
 */
bool Communicator::payload1TelemetryEnabled() const
{
    return m_payload1TelemetryEnabled;
}

/**
 * Returns @c true if SP2 telemetry is enabled
 */
bool Communicator::payload2TelemetryEnabled() const
{
    return m_payload2TelemetryEnabled;
}

/**
 * Returns @c true if container telemetry is enabled
 */
bool Communicator::containerTelemetryEnabled() const
{
    return m_containerTelemetryEnabled;
}

/**
 * Returns current time in hh:mm:ss:zzz format
 */
QString Communicator::currentTime() const
{
    return m_currentTime;
}

/**
 * Returns the name of the currently loaded CSV file
 */
QString Communicator::csvFileName() const
{
    if (m_file.isOpen())
    {
        auto fileInfo = QFileInfo(m_file.fileName());
        return fileInfo.fileName();
    }

    return tr("No CSV file selected");
}

/**
 * Returns the value of the current row of the simulated pressure CSV file
 */
QString Communicator::currentSimulatedReading() const
{
    return m_currentSimulationData;
}

/**
 * Opens a dialog that allows the user to select a CSV file to load to the application.
 * The CSV file must contain only one column with simulated pressure data.
 */
void Communicator::openCsv()
{
    // clang-format off
    auto name = QFileDialog::getOpenFileName(Q_NULLPTR,
                                             tr("Select simulation file"),
                                             QDir::homePath());
    // clang-format on

    // User did not select a file, abort
    if (name.isEmpty())
        return;

    // Close current file
    if (m_file.isOpen())
        m_file.close();

    // Open the selected file
    m_file.setFileName(name);
    if (m_file.open(QFile::ReadOnly))
    {
        // Close temp. file
        if (m_tempFile.isOpen())
            m_tempFile.close();

        // Disable simulation mode
        if (simulationActivated())
            setSimulationActivated(false);

        // Generate CSV data
        QString csv;
        QTextStream in(&m_file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            line.replace(" ", "");
            if (line.startsWith("#") || line.isEmpty())
                continue;
            else
            {
                line.replace("$", "1714");
                csv.append(line);
                csv.append("\n");
            }
        }

        // Save CSV file to temp path
        m_tempFile.setFileName(QDir::tempPath() + "/cc2021_temp.csv");
        if (m_tempFile.open(QFile::WriteOnly))
        {
            m_tempFile.write(csv.toUtf8());
            m_tempFile.close();
        }

        // Read CSV data
        if (m_tempFile.open(QFile::ReadOnly))
        {
            m_row = 0;
            m_currentSimulationData = "";
            m_csvData = QtCSV::Reader::readToList(m_tempFile);
            emit currentSimulatedReadingChanged();
        }
    }

    // Open failure, alert user through a messagebox
    else
        Misc::Utilities::showMessageBox(tr("File open error"), m_file.errorString());

    // Update UI
    emit csvFileNameChanged();
}

/**
 * Tries to establish a connection with Serial Studio's TCP server
 */
void Communicator::tryConnection()
{
    if (!connectedToSerialStudio())
    {
        m_socket.abort();
        m_socket.connectToHost(QHostAddress::LocalHost, SERIAL_STUDIO_PLUGINS_PORT);
    }
}

/**
 * Sends the command to release the first scientific payload
 */
void Communicator::releasePayload1()
{
    if (connectedToSerialStudio())
        sendData("CMD,1714,SP,R1;");
}

/**
 * Sends the command to release the second scientific payload
 */
void Communicator::releasePayload2()
{
    if (connectedToSerialStudio())
        sendData("CMD,1714,SP,R2;");
}

/**
 * Sends the current time to the payload with the hh:mm:ss format
 */
void Communicator::updateContainerTime()
{
    if (connectedToSerialStudio())
    {
        auto time = QDateTime::currentDateTime().toString("hh:mm:ss");
        sendData("CMD,1714,ST," + time + ";");
    }
}

/**
 * Enables/disables simulation mode
 */
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

/**
 * Activates/deactivates sending simulated pressure readings to the CanSat
 */
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

/**
 * Enables/disables SP1 telemetry
 */
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

/**
 * Enables/disables SP2 telemetry
 */
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

/**
 * Enables/disables container telemetry
 */
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

/**
 * Gets the current time in hh:mm:ss:zzz format. This value is used by the user interface,
 * not by the CanSat container.
 */
void Communicator::updateCurrentTime()
{
    m_currentTime = QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
    emit currentTimeChanged();
}

/**
 * Reads, validates & sends current simulated pressure reading to the CanSat.
 *
 * If we reach the last CSV row, then simulation mode shall be disabled & a message-box
 * shall be shown to the user.
 */
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

        // Generate row string
        QString cmd = "";
        for (int i = 0; i < row.count(); ++i)
        {
            cmd.append(row.at(i));
            cmd.append(",");
        }

        // Remove last "," from command string & add ";"
        cmd.chop(1);
        cmd.append(";");

        // Send command
        if (!cmd.isEmpty())
            sendData(cmd);

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
        setSimulationActivated(false);
        Misc::Utilities::showMessageBox(tr("Pressure simulation finished"),
                                        tr("Reached end of CSV file"));
    }
}

/**
 * Waits 500 ms and notifies the UI if the TCP connection with Serial Studio has been
 * established.
 *
 * We need to wait in order to avoid 'flickering' in the UI when the plugin system of
 * Serial Studio is disabled.
 */
void Communicator::onConnectedChanged()
{
    QTimer::singleShot(500, this, &Communicator::connectedChanged);
}

/**
 * Displays any socket errors with a message-box
 */
void Communicator::onErrorOccurred(const QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    Misc::Utilities::showMessageBox(tr("TCP socket error"), m_socket.errorString());
}

/**
 * Sends the given @a data string to Serial Studio, which in turn sends the data through
 * the serial port.
 */
bool Communicator::sendData(const QString &data)
{
    if (connectedToSerialStudio() && !data.isEmpty())
    {
        // Add extra bytes to generate fixed-length string
        QString copy = data;
        while (copy.length() < 22)
            copy.append("\n");

        // Write data to TCP socket
        auto bytes = copy.toUtf8();
        auto wleng = m_socket.write(bytes);

        // Calculate length of data sent
        auto sentData = bytes;
        sentData.chop(bytes.length() - wleng);
        sentData.replace("\n", "");

        // Update UI
        emit rx("TX: " + QString::fromUtf8(sentData) + "\n");
        return sentData.length() == bytes.length();
    }

    return false;
}
