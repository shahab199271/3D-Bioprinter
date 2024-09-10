#code by Shahab Baloochi



#include "marlinimpl.h"
#include "raspberrydevice.h"
#include "MarlinConfig.h"
#include "gcode.h"
#include "gcodepreprocessor.h"
#include <QDebug>
#include <QLineF>
#include <QRegularExpression>
#include <QSerialPortInfo>
#include <chrono>

MarlinImpl::MarlinImpl(RaspberryDevice *dev, QObject *parent)
    : QObject(parent),
      m_raspberry(dev)
{
    connect(this, &MarlinImpl::temperatureSignal, m_raspberry, &RaspberryDevice::readTemperature);
    connect(this, &MarlinImpl::printProgress, m_raspberry, &RaspberryDevice::printProgress);
    connect(this, &MarlinImpl::printFinished, m_raspberry, &RaspberryDevice::printFinished);
    connect(this, &MarlinImpl::startCaptureTimer, m_raspberry, &RaspberryDevice::startCaptureTimer);
    connect(this, &MarlinImpl::halted, m_raspberry, &RaspberryDevice::halted);
    connect(this, &MarlinImpl::temperatureSignal, [=](int idx, qreal cur, qreal /*set*/) {
        if(idx >= 0 && idx <= 2)
            m_currentTemp[idx] = cur;
    });
}

bool MarlinImpl::send()
{
    QString temp = m_gcodesForSend.join("\n") + "\n";
    qDebug().noquote() << temp;
    m_port.write(temp.toUtf8());
    m_gcodesForSend.clear();
    return true;
}

MarlinImpl &MarlinImpl::bedLevelingBegin()
{
    return enqueue(G29(1));
}

MarlinImpl &MarlinImpl::bedLevelingNext()
{
    return enqueue(G29(2));
}

MarlinImpl &MarlinImpl::bedLevelingSetPoint(int i, int j, qreal z)
{
    return enqueue(G29(i, j, z));
}

MarlinImpl &MarlinImpl::saveDataToFlash()
{
    return enqueue(M500());
}

MarlinImpl &MarlinImpl::setHotendOffset(int nozzle, qreal x, qreal y)
{
    return enqueue(M218_XY(nozzle, x, y));
}

MarlinImpl &MarlinImpl::setNozzleZOffset(int nozzle, double deltaFrom5)
{
    return enqueue(M218_Z(nozzle, -deltaFrom5));
}

MarlinImpl &MarlinImpl::setNozzlesZHomeOffset(double value)
{
    return enqueue(M206(value));
}


#Extended Code with Temperature Logging:
#Shahab baloochi

#include "marlinimpl.h"
#include "raspberrydevice.h"
#include "MarlinConfig.h"
#include "gcode.h"
#include "gcodepreprocessor.h"
#include <QDebug>
#include <QLineF>
#include <QRegularExpression>
#include <QSerialPortInfo>
#include <chrono>
#include <QFile>
#include <QTextStream>

MarlinImpl::MarlinImpl(RaspberryDevice *dev, QObject *parent)
    : QObject(parent),
      m_raspberry(dev),
      m_isLogging(false) // Initialize logging flag
{
    connect(this, &MarlinImpl::temperatureSignal, m_raspberry, &RaspberryDevice::readTemperature);
    connect(this, &MarlinImpl::printProgress, m_raspberry, &RaspberryDevice::printProgress);
    connect(this, &MarlinImpl::printFinished, m_raspberry, &RaspberryDevice::printFinished);
    connect(this, &MarlinImpl::startCaptureTimer, m_raspberry, &RaspberryDevice::startCaptureTimer);
    connect(this, &MarlinImpl::halted, m_raspberry, &RaspberryDevice::halted);

    // Temperature monitoring signal
    connect(this, &MarlinImpl::temperatureSignal, [=](int idx, qreal cur, qreal /*set*/) {
        if(idx >= 0 && idx <= 2)
            m_currentTemp[idx] = cur;
        
        if (m_isLogging) {
            logTemperature(idx, cur);
        }
    });
}

bool MarlinImpl::send()
{
    QString temp = m_gcodesForSend.join("\n") + "\n";
    qDebug().noquote() << temp;
    m_port.write(temp.toUtf8());
    m_gcodesForSend.clear();
    return true;
}

MarlinImpl &MarlinImpl::bedLevelingBegin()
{
    return enqueue(G29(1));
}

MarlinImpl &MarlinImpl::bedLevelingNext()
{
    return enqueue(G29(2));
}

MarlinImpl &MarlinImpl::bedLevelingSetPoint(int i, int j, qreal z)
{
    return enqueue(G29(i, j, z));
}

MarlinImpl &MarlinImpl::saveDataToFlash()
{
    return enqueue(M500());
}

MarlinImpl &MarlinImpl::setHotendOffset(int nozzle, qreal x, qreal y)
{
    return enqueue(M218_XY(nozzle, x, y));
}

MarlinImpl &MarlinImpl::setNozzleZOffset(int nozzle, double deltaFrom5)
{
    return enqueue(M218_Z(nozzle, -deltaFrom5));
}

MarlinImpl &MarlinImpl::setNozzlesZHomeOffset(double value)
{
    return enqueue(M206(value));
}

// New feature: Temperature Logging
void MarlinImpl::startTemperatureLogging(const QString &filePath)
{
    if (m_isLogging) {
        qDebug() << "Temperature logging is already in progress.";
        return;
    }

    m_logFile.setFileName(filePath);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for temperature logging.";
        return;
    }

    m_isLogging = true;
    qDebug() << "Started temperature logging to" << filePath;
}

void MarlinImpl::stopTemperatureLogging()
{
    if (!m_isLogging) {
        qDebug() << "Temperature logging is not in progress.";
        return;
    }

    m_logFile.close();
    m_isLogging = false;
    qDebug() << "Stopped temperature logging.";
}

void MarlinImpl::logTemperature(int idx, qreal temp)
{
    if (!m_isLogging || !m_logFile.isOpen()) {
        return;
    }

    QTextStream out(&m_logFile);
    QString tempType = (idx == 0) ? "Hotend" : (idx == 1) ? "Bed" : "Unknown";
    out << QDateTime::currentDateTime().toString(Qt::ISODate) 
        << ": " << tempType << " Temperature: " << temp << "°C\n";
}

QString MarlinImpl::retrieveLog() const
{
    if (!m_logFile.isOpen()) {
        return "No log available. Logging is either stopped or file is not accessible.";
    }

    QFile log(m_logFile.fileName());
    if (!log.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "Failed to read log file.";
    }

    QTextStream in(&log);
    return in.readAll();
}


#Start Logging
#marlinImpl.startTemperatureLogging("/path/to/logfile.txt");

#Stop Logging
#marlinImpl.stopTemperatureLogging();

#Retrieve Log
#QString logContent = marlinImpl.retrieveLog();
#qDebug() << logContent;





