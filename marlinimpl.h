#ifndef MARLINIMPL_H
#define MARLINIMPL_H

#include <QVariant>
#include <QSerialPort>
#include <QTimer>
#include <QVector>
#include "gcodefunctions.h"
#include "gcodeprocessor.h"
#include "optional.hpp"

class RaspberryDevice;


class MarlinImpl : public QObject, private GCodeFunctions
{
    Q_OBJECT

    QSerialPort m_port;
    RaspberryDevice *m_raspberry;
    GCodeProcessor m_gcodeProcessor;
    QStringList m_gcodesForSend;
    QStringList m_printGCodeLines;

public:
    MarlinImpl(RaspberryDevice *dev, QObject *parent = nullptr);

    bool send();

    MarlinImpl &bedLevelingBegin();
    MarlinImpl &bedLevelingNext();
    MarlinImpl &bedLevelingSetPoint(int i, int j, qreal z);
    MarlinImpl &saveDataToFlash();
    MarlinImpl &setHotendOffset(int nozzle, qreal x, qreal y);
    MarlinImpl &setNozzleZOffset(int nozzle, double deltaFrom5);
    MarlinImpl &setNozzlesZHomeOffset(double value);

signals:
    void temperatureSignal(int nozzleOrBedIndex, qreal current, qreal set);
    void printProgress(int value);
    void printFinished();
    void startCaptureTimer(int seconds);
    void halted();
};

#endif // MARLINIMPL_H
