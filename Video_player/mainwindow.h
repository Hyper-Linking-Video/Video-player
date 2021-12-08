#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "Image.h"
#include <QLabel>
#include <QMediaPlayer>
#include <vector>
#include <unordered_map>
#include <QString>

struct SecondImage
{
    int PrimaryVideoFrameNumber;
    int AimVideoFrameNumber;
    QString AimVideoName;
    QString HyperLinkName;
    int BottomRightPointX;
    int BottomRightPointY;
    int TopLeftPointX;
    int TopLeftPointY;
};

struct MainImage
{
    int iId;
    QString qsImagePath;
    std::unordered_map<QString,std::vector<int>> qsUrl;//second info
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
    void LoadImage(int imageid,QString imagePath);

private slots:
    void mouseReleaseEvent(QMouseEvent *e);
    void UpdateTimer(int offset=0);
    void OnTimerSwitch();
    bool getSecondData(const QString &data_path);
    void AddLink();
    void ShowImage();
    void on_LoadVideoButton_clicked();
    void on_PlayButton_clicked(bool checked);
    void on_verticalSlider_valueChanged(int value);
    void on_PauseButton_clicked(bool checked);
    void LoadSecond(QString dir_name,int n);
    void Clear();

    void on_RestartButton_clicked();

protected:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *SoundPlayer;
    QAudioOutput *audioOutput;
    QTimer* Timerswitch;
    int m_waitingtime=33; // faster than thought
    std::unordered_map<int, MainImage *> ImageArray;
    std::vector<SecondImage *> JsonArray;
    int CurrentId;
};
#endif // MAINWINDOW_H
