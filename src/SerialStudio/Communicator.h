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

#ifndef SERIALSTUDIO_COMMUNICATOR_H
#define SERIALSTUDIO_COMMUNICATOR_H

#include <QFile>
#include <QObject>
#include <QTcpSocket>

namespace SerialStudio
{
class Communicator : public QObject
{
    // clang-format off
    Q_OBJECT
    Q_PROPERTY(bool connectedToSerialStudio
               READ connectedToSerialStudio
               NOTIFY connectedChanged)
    Q_PROPERTY(bool simulationEnabled
               READ simulationEnabled
               WRITE setSimulationMode
               NOTIFY simulationEnabledChanged)
    Q_PROPERTY(bool simulationActivated
               READ simulationActivated
               WRITE setSimulationActivated
               NOTIFY simulationActivatedChanged)
    Q_PROPERTY(bool payload1TelemetryEnabled
               READ payload1TelemetryEnabled
               WRITE setPayload1TelemetryEnabled
               NOTIFY payload1TelemetryEnabledChanged)
    Q_PROPERTY(bool payload2TelemetryEnabled
               READ payload2TelemetryEnabled
               WRITE setPayload2TelemetryEnabled
               NOTIFY payload2TelemetryEnabledChanged)
    Q_PROPERTY(bool containerTelemetryEnabled
               READ containerTelemetryEnabled
               WRITE setContainerTelemetryEnabled
               NOTIFY containerTelemetryEnabledChanged)
    Q_PROPERTY(QString currentTime
               READ currentTime
               NOTIFY currentTimeChanged)
    Q_PROPERTY(QString csvFileName
               READ csvFileName
               NOTIFY csvFileNameChanged)
    Q_PROPERTY(QString currentSimulatedReading
               READ currentSimulatedReading
               NOTIFY currentSimulatedReadingChanged)
    // clang-format on

signals:
    void currentTimeChanged();
    void csvFileNameChanged();
    void simulationEnabledChanged();
    void simulationActivatedChanged();
    void connectedChanged();
    void currentSimulatedReadingChanged();
    void payload1TelemetryEnabledChanged();
    void payload2TelemetryEnabledChanged();
    void containerTelemetryEnabledChanged();
    void rx(const QString &data);

public:
    static Communicator *getInstance();

    bool connectedToSerialStudio() const;

    bool simulationEnabled() const;
    bool simulationActivated() const;
    bool payload1TelemetryEnabled() const;
    bool payload2TelemetryEnabled() const;
    bool containerTelemetryEnabled() const;

    QString currentTime() const;
    QString csvFileName() const;
    QString currentSimulatedReading() const;

public slots:
    void openCsv();
    void tryConnection();
    void releasePayload1();
    void releasePayload2();
    void updateContainerTime();
    void setSimulationMode(const bool enabled);
    void setSimulationActivated(const bool activated);
    void setPayload1TelemetryEnabled(const bool enabled);
    void setPayload2TelemetryEnabled(const bool enabled);
    void setContainerTelemetryEnabled(const bool enabled);

private slots:
    void updateCurrentTime();
    void sendSimulatedData();
    void onConnectedChanged();
    void onErrorOccurred(const QAbstractSocket::SocketError socketError);

private:
    Communicator();
    bool sendData(const QString &data);

private:
    QTcpSocket m_socket;

    int m_row;
    QFile m_csvFile;
    QString m_currentTime;
    QList<QStringList> m_csvData;
    QString m_currentSimulationData;

    bool m_simulationEnabled;
    bool m_simulationActivated;
    bool m_payload1TelemetryEnabled;
    bool m_payload2TelemetryEnabled;
    bool m_containerTelemetryEnabled;
};
};

#endif
