#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QAudioOutput>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QThread>
#include <QTimer>
#include <algorithm>
#include <QElapsedTimer>

#include "myqlabel.h"

//MyImage VideoFrame[9000];

// better way to show x,y?

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Window");
    ui->PlayButton->setEnabled(false);
    ui->PauseButton->setEnabled(false);
    ui->RestartButton->setEnabled(false);
    ui->verticalSlider->setEnabled(false);
    //Cursor check
    ui->label_statue->setText("");
    ui->label_X->setText("X: ");
    ui->label_Y->setText("Y: ");
    this->setMouseTracking(true);
    connect(this->ui->Video, SIGNAL(clicked(bool)), this, SLOT(mouseReleaseEvent(QMouseEvent * e)));
    //image
    CurrentId = 0;
    Timerswitch = new QTimer(this);
    Timerswitch->setSingleShot(true);
    //Timerswitch->setInterval(m_waitingtime);
    //Newthread = new QThread(this);
    //Timerswitch->moveToThread(Newthread);
    //connect(Newthread, SIGNAL(started()), Timerswitch, SLOT(start()));
    //QTimer::singleShot(m_waitingtime, this,SLOT(OnTimerSwitch()));
    connect(Timerswitch, SIGNAL(timeout()), this, SLOT(OnTimerSwitch()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::UpdateTimer(int offset)
{
    Timerswitch->stop();
    if(m_waitingtime-offset<0){
        ShowImage();
        Timerswitch->start(m_waitingtime);
    }
    else{
        Timerswitch->start(m_waitingtime-offset);
    }
}
void MainWindow::OnTimerSwitch()
{
    QElapsedTimer timedebuge;
    timedebuge.start();
    CurrentId=SoundPlayer->position()*ImageArray.size()/SoundPlayer->duration();
    if (CurrentId >= ImageArray.size())
    {
        //QImage image(data,width,height,每行字节数,QImage::Format_RGB888);
        //nextimg=QPixmap::fromImage(image);
        //ui->Video->setPixmap(QPixmap::fromImage(image));
        //nextimg.load(ImageArray[0]->qsImagePath);
        statusBar()->showMessage(tr("Finish"));
        Timerswitch->stop();
        SoundPlayer->stop();
    }
    else
    {
        if (ui->PauseButton->isEnabled())
        {
            ShowImage();
        }
        //nextimg.load(ImageArray[CurrentId]->qsImagePath);
    }
    //qDebug()<<"show one image need: "<<timedebuge.elapsed()<<"ms";
    //Newthread->start();
    this->UpdateTimer(timedebuge.elapsed());
}
void MainWindow::ShowImage()
{
    //QElapsedTimer timedebuge;
    //timedebuge.start();
    MyImage VideoFrame;
    VideoFrame.setWidth(352);
    VideoFrame.setHeight(288);
    auto pathTemp = ImageArray[CurrentId]->qsImagePath.toLatin1();
    VideoFrame.setImagePath(pathTemp.data());
    unsigned char *VideoFrameTemp = new unsigned char[3 * 352 * 288]();
    VideoFrame.ReadImage();
    VideoFrame.setFlag();
    VideoFrameTemp = (unsigned char *)VideoFrame.getImageData();
    QImage *tempImage = new QImage(VideoFrameTemp, 352, 288, QImage::Format_RGB888);
    ui->Video->setAlignment(Qt::AlignLeft);
    ui->Video->setAlignment(Qt::AlignTop);
    ui->Video->setPixmap(QPixmap::fromImage(*tempImage));
    //qDebug()<<"show one image need: "<<timedebuge.elapsed()<<"ms";
    ui->Video->clearAreaList();
    if (ImageArray[CurrentId]->qsUrl.size() != 0){
        statusBar()->showMessage(tr("Have Link in frame")+QString::number(CurrentId));
        setCursor(Qt::CrossCursor);
        for(auto link:ImageArray[CurrentId]->qsUrl){
            ui->Video->setNewPainter(link.second);
        }
    }
    else{
        statusBar()->showMessage(QString::number(CurrentId));
        setCursor(Qt::ArrowCursor);
    }
}
bool MainWindow::getSecondData(const QString &data_path)
{
    QFile load_file(data_path);
    if (!load_file.open(QFile::ReadOnly))
    {
        qWarning("Couldn't open json file.");
        return false;
    }
    QByteArray data = load_file.readAll();
    QJsonParseError err;
    QJsonDocument json_doc = QJsonDocument::fromJson(data, &err);
    if (json_doc.isNull())
    {
        qDebug() << err.errorString();
        return false;
    }
    QJsonObject echo_info = json_doc.object();

    if (echo_info.contains("Links") && echo_info["Links"].isArray())
    {
        QJsonArray info_array = echo_info["Links"].toArray();
        for (int i = 0; i < info_array.size(); i++)
        {
            QJsonObject item_object = info_array[i].toObject();

            qreal AimVideoFrameNumber = -1;
            qreal BottomRightPointX = -1;
            qreal BottomRightPointY = -1;
            qreal TopLeftPointX = -1;
            qreal TopLeftPointY = -1;
            qreal PrimaryVideoFrameNumber = -1;
            QString AimVideoName;
            QString HyperLinkName;

            if (item_object.contains("AimVideoFrameNumber") && item_object["AimVideoFrameNumber"].toInt())
            {
                AimVideoFrameNumber = item_object["AimVideoFrameNumber"].toInt();
            }
            if (item_object.contains("PrimaryVideoFrameNumber") && item_object["PrimaryVideoFrameNumber"].toInt())
            {
                PrimaryVideoFrameNumber = item_object["PrimaryVideoFrameNumber"].toInt();
            }
            if (item_object.contains("BottomRightPointX") && item_object["BottomRightPointX"].toInt())
            {
                BottomRightPointX = item_object["BottomRightPointX"].toInt();
            }
            if (item_object.contains("TopLeftPointX") && item_object["TopLeftPointX"].toInt())
            {
                TopLeftPointX = item_object["TopLeftPointX"].toInt();
            }
            if (item_object.contains("TopLeftPointY") && item_object["TopLeftPointY"].toInt())
            {
                TopLeftPointY = item_object["TopLeftPointY"].toInt();
            }
            if (item_object.contains("BottomRightPointY") && item_object["BottomRightPointY"].toInt())
            {
                BottomRightPointY = item_object["BottomRightPointY"].toInt();
            }
            if (item_object.contains("AimVideoName"))
            {
                AimVideoName = item_object["AimVideoName"].toString();
            }
            if (item_object.contains("HyperLinkName"))
            {
                HyperLinkName = item_object["HyperLinkName"].toString();
            }
            if (BottomRightPointX >= TopLeftPointX && BottomRightPointY >= TopLeftPointY && PrimaryVideoFrameNumber > 0 && AimVideoFrameNumber > 0)
            {
                SecondImage *img = new SecondImage;
                img->AimVideoFrameNumber = AimVideoFrameNumber;
                img->PrimaryVideoFrameNumber = PrimaryVideoFrameNumber;
                img->BottomRightPointX = BottomRightPointX;
                img->BottomRightPointY = BottomRightPointY;
                img->TopLeftPointX = TopLeftPointX;
                img->TopLeftPointY = TopLeftPointY;
                img->AimVideoName = AimVideoName;
                img->HyperLinkName = HyperLinkName;
                JsonArray.push_back(img);
            }
            else
            {
                qWarning("data read error");
                return false;
            }
        }
    }

    return true;
}

void MainWindow::LoadImage(int imageid, QString imagePath)
{
    MainImage *img = new MainImage;
    img->iId = imageid;
    img->qsImagePath = imagePath;
    ImageArray[imageid] = img;
    return;
}
void MainWindow::AddLink()
{
    //int t=0;
    for (auto info : JsonArray)
    {
        auto got = ImageArray.find(info->PrimaryVideoFrameNumber);
        if (got != ImageArray.end())
        {
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl[info->AimVideoName].push_back(info->AimVideoFrameNumber);
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl[info->AimVideoName].push_back(info->TopLeftPointX);
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl[info->AimVideoName].push_back(info->TopLeftPointY);
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl[info->AimVideoName].push_back(info->BottomRightPointX);
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl[info->AimVideoName].push_back(info->BottomRightPointY);
            //t=t+1;
        }
    }
    //qDebug() << "add links:" << t;
}

void MainWindow::on_LoadVideoButton_clicked()
{
    QString dir_name = QFileDialog::getExistingDirectory(NULL, "Please choose the directory with the primary video's frame files", ".");
    LoadSecond(dir_name,0);
    if(dir_name.size() > 0){
        ui->PlayButton->setEnabled(true);
        ui->RestartButton->setEnabled(true);
        ui->PauseButton->setEnabled(false);
        ui->verticalSlider->setEnabled(true);
    }
}
void MainWindow::LoadSecond(QString dir_name,int n){
    statusBar()->showMessage(tr("Loading"));
    Clear();
    CurrentId=n;
    qDebug() << "Dir Path:" << dir_name;
    if (dir_name.size() > 0)
    {
        QDir *inputDir = new QDir(dir_name);
        QStringList filter;
        filter << "*.rgb";
        inputDir->setNameFilters(filter);
        QList<QFileInfo> *fileInfo = new QList<QFileInfo>(inputDir->entryInfoList(filter));
        int fileCount = fileInfo->count();
        qDebug() << fileCount;

        if (fileCount < 9000)
        {
            QMessageBox::warning(this, tr("Error!"), tr("The frame files in this directory are not enough to generate a video. Please choose another directory."));
            return;
        }
        for (int i = 0; i < fileCount; i++)
        {
            QString imagePath = fileInfo->at(i).filePath();
            LoadImage(i, imagePath);
        }
        statusBar()->showMessage(tr("Load json"));
        QDir *data_path = new QDir(dir_name);
        QStringList jsonfilter; //filter can not reuse
        jsonfilter << "*.json";
        data_path->setNameFilters(jsonfilter);
        QList<QFileInfo> *datafile = new QList<QFileInfo>(data_path->entryInfoList(jsonfilter));
        int datafileCount = datafile->count();
        if(datafileCount>0){
            QString jsonPath = datafile->at(0).filePath();
            getSecondData(jsonPath);
            qDebug() << "JsonArray size:" << JsonArray.size();
            AddLink();
        }
        statusBar()->showMessage(tr("Sound"));
        SoundPlayer = new QMediaPlayer;
        audioOutput = new QAudioOutput;
        SoundPlayer->setAudioOutput(audioOutput);
        QDir *videoPath = new QDir(dir_name);
        QStringList wavfilter;
        wavfilter << "*.wav";
        videoPath->setNameFilters(wavfilter);
        QList<QFileInfo> *wavfile = new QList<QFileInfo>(videoPath->entryInfoList(wavfilter));
        QString filepath = wavfile->at(0).filePath();
        qDebug() << filepath;
        SoundPlayer->setSource(QUrl::fromLocalFile(wavfile->at(0).filePath()));
        audioOutput->setVolume(double(ui->verticalSlider->value())/100.);
        ui->PlayButton->setEnabled(false);
        ui->PauseButton->setEnabled(true);
        ui->RestartButton->setEnabled(true);
        //connect(SoundPlayer, &QMediaPlayer::durationChanged, this, [&](qint64 dur) { qDebug() << "duration = " << dur; });
        statusBar()->showMessage(tr("Done"));
        ShowImage();
    }
}

void MainWindow::on_RestartButton_clicked()
{
    SoundPlayer->stop();
    Timerswitch->stop();
    CurrentId=0;
    ShowImage();
    SoundPlayer->setPosition(SoundPlayer->duration()/9000*CurrentId);
    //Newthread->quit();
    ui->PlayButton->setEnabled(false);
    ui->PauseButton->setEnabled(true);
    SoundPlayer->play();
    Timerswitch->start();
    statusBar()->showMessage(tr("RESTART"));
}
void MainWindow::on_PlayButton_clicked(bool checked)
{
    SoundPlayer->play();
    Timerswitch->start();
    //Newthread->start();
    ui->PauseButton->setEnabled(true);
    ui->PlayButton->setEnabled(false);
}

void MainWindow::on_PauseButton_clicked(bool checked)
{
    Timerswitch->stop();
    //Newthread->quit();
    SoundPlayer->pause();
    ui->PlayButton->setEnabled(true);
    ui->PauseButton->setEnabled(false);
}
void MainWindow::on_verticalSlider_valueChanged(int value)
{
    double New=double(value)/100.0;
    audioOutput->setVolume(New);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->pos(); //Cursor position
    int X=p.x()-ui->Video->x();
    int Y=p.y()-ui->Video->y();
    if(X<350 && Y<290){
        ui->label_X->setText("X: " + QString::number(X));
        ui->label_Y->setText("Y: " + QString::number(Y));
    }
    else{
        ui->label_X->setText("Out of boundary");
        ui->label_Y->setText("Out of boundary");
    }
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint p = event->pos(); //Cursor position
    int X=p.x()-ui->Video->x();
    int Y=p.y()-ui->Video->y();
    if (event->button() == Qt::LeftButton)
    {
        ui->label_statue->setText("Left Button Pressed");
        if(X<350 && Y<290 && X>0 && Y>0){
            ui->label_X->setText("X: " + QString::number(X));
            ui->label_Y->setText("Y: " + QString::number(Y));
        }
        else{
            ui->label_X->setText("Out of boundary");
            ui->label_Y->setText("Out of boundary");
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        ui->label_statue->setText("Right Button Pressed");
        if(X<350 && Y<287 && X>0 && Y>0){
            ui->label_X->setText("X: " + QString::number(X));
            ui->label_Y->setText("Y: " + QString::number(Y));
        }
        else{
            ui->label_X->setText("Out of boundary");
            ui->label_Y->setText("Out of boundary");
        }
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event) // release
{
    QPoint p = event->pos();
    int X=p.x()-ui->Video->x();
    int Y=p.y()-ui->Video->y();
    ui->label_statue->setText("Mouse Released");
    if(X<350 && Y<287 && X>0 && Y>0){
        ui->label_X->setText("X: " + QString::number(X));
        ui->label_Y->setText("Y: " + QString::number(Y));
    }
    else{
        ui->label_X->setText("Out of boundary");
        ui->label_Y->setText("Out of boundary");
    }
    if(ImageArray.size()>0 && Y<287 && X<350 && X>0 && Y>0)
    {
        if (ImageArray[CurrentId]->qsUrl.size() != 0)
        {
            int X=p.x()-ui->Video->x();
            int Y=p.y()-ui->Video->y();
            for(auto link:ImageArray[CurrentId]->qsUrl){
                for(auto i=0;i<link.second.size();i=i+5){
                    int left = link.second[i+1]; //X
                    int right = link.second[i+3];
                    int top = link.second[i+2]; //Y
                    int bottom = link.second[i+4];
    //                qDebug() << "left:" << left;
    //                qDebug() << "right:" << right;
    //                qDebug() << "top:" << top;
    //                qDebug() << "bottom:" << bottom;
                    if ((X > left) && (X < right) && (Y > top) && (Y < bottom))
                    {
                        //LoadImage
                        ui->label_statue->setText("Next Video");
                        //Newthread->quit();
                        Timerswitch->stop();
                        SoundPlayer->stop();
                        qDebug() <<"Next Video at frame "<<link.second[i];
                        LoadSecond(link.first,link.second[i]);
                        QObject::connect(SoundPlayer, &QMediaPlayer::mediaStatusChanged,
                                             [&](QMediaPlayer::MediaStatus status){
                                if(status == QMediaPlayer::LoadedMedia) {
                                    SoundPlayer->setPosition(SoundPlayer->duration()*CurrentId/9000);
                                    SoundPlayer->play();
                                    Timerswitch->start();
                                    SoundPlayer->disconnect(); //!!! will cause error if not disconnect
                                    //Newthread->start();
                                }
                        });
                    }
                }
            }
        }
    }
}
void MainWindow::Clear(){
    ImageArray.clear();
    JsonArray.clear();
    CurrentId=0;
    ui->Video->clearAreaList();
}

