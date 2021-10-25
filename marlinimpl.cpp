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






