#include "mythread.h"

MyThread::MyThread()
{

}

MyThread::~MyThread()
{

}

void MyThread::run()
{
    int i = 0;
    int t = 0;
    timer.start();
    int ms = 0;
    while(i<=9000){
        while(t>timer.elapsed())
            msleep(1);
        emit needNew();
        i++;
        if(i%30==0){
            t+=43;
        }
        else{
            t+=33;
        }
    }
    qDebug()<<i;
    emit isDone();
}
