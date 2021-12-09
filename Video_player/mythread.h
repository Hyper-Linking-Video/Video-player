#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QTimer>
#include <QDebug>

class MyThread : public QThread
{
     Q_OBJECT
public:
    explicit MyThread();
    ~MyThread();

protected:
    void run();

signals:
    void needNew();
    void isDone();

private:
    QElapsedTimer timer;
};

#endif // MYTHREAD_H
