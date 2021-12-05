#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "Image.h"
#include <QLabel>
#include <QMediaPlayer>
#include <vector>

struct MainImage
{
    int iId;
    QString qsImagePath;
    QString qsUrl;//second info
};

struct SecondImage
{
    int AimVideoFrameNumber;
    int PrimaryVideoFrameNumber;
    int BottomRightPointX;
    int BottomRightPointY;
    int TopLeftPointX;
    int TopLeftPointY;
};

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int LoadImage(QString imagePath, QString url = NULL);
    void switchToImg(int index);

private slots:
    void on_LoadVideoButton_clicked();
    void on_PlayButton_clicked(bool checked);
    void mouseReleaseEvent(QMouseEvent *e);
    void UpdateTimer();
    void OnTimerSwitch();

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    Ui::MainWindow *ui;
    QLabel *labelStatus;
    QLabel *labelMousePos;
    QMediaPlayer Sound;
    QTimer* Timerswitch;
    int m_waitingtime=333;
    int imageid;
    std::vector<MainImage *> ImageArray;
    int m_currentindex;
};
#endif // MAINWINDOW_H
