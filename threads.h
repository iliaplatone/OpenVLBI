/*
    MIT License

    libahp_xc library to drive the AHP XC correlators
    Copyright (C) 2020  Ilia Platone

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef THREADS_H
#define THREADS_H

#include <cmath>
#include <QThread>
#include <QWidget>
#include <QDateTime>
#include <QMutex>

class Thread : public QThread
{
    Q_OBJECT
private:
    QObject* parent;
    QMutex mutex;
    int next_ms;
    int timer_ms;
    int loop_ms;
    QString Name;
    bool running { false };
public:
    Thread(QObject* p = nullptr, int timer = 20, int loop = 20, QString n = "") : QThread()
    {
        parent = p;
        timer_ms = timer;
        loop_ms = loop;
        next_ms = timer_ms;
        Name = n;
    }
    void run()
    {
        running = true;
        lastPollTime = QDateTime::currentDateTimeUtc();
        while(!isInterruptionRequested())
        {
            lastPollTime = QDateTime::currentDateTimeUtc();
            next_ms = loop_ms;
            if(lock())
            {
                emit threadLoop(this);
                QThread::msleep(next_ms);
            }
        }
        running = false;
    }
    void stop()
    {
        requestInterruption();
        next_ms = timer_ms;
    }
    bool lock()
    {
        return mutex.tryLock(loop_ms);
    }
    void unlock()
    {
        mutex.unlock();
    }
    void setTimer(int timer)
    {
        timer_ms = timer;
    }
    void setLoop(int loop)
    {
        loop_ms = loop;
    }
    QString getName()
    {
        return Name;
    }
    QObject *getParent() { return parent; }
private:
    QDateTime lastPollTime;
signals:
    void threadLoop(Thread *);
};

#endif // THREADS_H
