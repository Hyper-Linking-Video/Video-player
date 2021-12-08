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
#include <QTimer>
#include <algorithm>
#include <QElapsedTimer>

//MyImage VideoFrame[9000];

// more visualize links?
// better way to show x,y?
// more accurate time？
// volumn control ?
//Q: 多线程？（button，image_time，audio，click_check);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Window1");
    ui->PlayButton->setEnabled(false);
    ui->PauseButton->setEnabled(false);
    ui->RestartButton->setEnabled(false);
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
    //QTimer::singleShot(m_waitingtime, this,SLOT(OnTimerSwitch()));
    connect(Timerswitch, SIGNAL(timeout()), this, SLOT(OnTimerSwitch()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::UpdateTimer()
{
    Timerswitch->stop();
    Timerswitch->start(m_waitingtime);
}
void MainWindow::OnTimerSwitch()
{
    //QElapsedTimer timedebuge;
    //timedebuge.start();
    this->UpdateTimer();
    CurrentId++;
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
    if (ImageArray[CurrentId]->qsUrl.size() != 0){
        statusBar()->showMessage(tr("Have Link in frame")+QString::number(CurrentId));
        setCursor(Qt::CrossCursor);
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
    int t=0;
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
            t=t+1;
        }
    }
    qDebug() << "add links:" << t;
}

void MainWindow::on_LoadVideoButton_clicked()
{
    statusBar()->showMessage(tr("Loading"));
    Clear();
    QString dir_name = QFileDialog::getExistingDirectory(NULL, "Please choose the directory with the primary video's frame files", ".");
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
        QDir *data_path = new QDir(dir_name);
        QStringList jsonfilter;
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
        audioOutput->setVolume(50);

        ui->PlayButton->setEnabled(true);
        ui->RestartButton->setEnabled(true);
        ui->PauseButton->setEnabled(false);
        statusBar()->showMessage(tr("Done"));
        ShowImage();
    }
}

void MainWindow::on_PlayButton_clicked(bool checked)
{
    SoundPlayer->play();
    Timerswitch->start();
    ui->PauseButton->setEnabled(true);
    ui->PlayButton->setEnabled(false);
}

void MainWindow::on_PauseButton_clicked(bool checked)
{
    Timerswitch->stop();
    SoundPlayer->pause();
    ui->PlayButton->setEnabled(true);
    ui->PauseButton->setEnabled(false);
}
void MainWindow::on_verticalSlider_valueChanged(int value)
{
    audioOutput->setVolume(value);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->pos(); //Cursor position
    int X=p.x()-ui->Video->x();
    int Y=p.y()-ui->Video->y();
    ui->label_X->setText("X: " + QString::number(X));
    ui->label_Y->setText("Y: " + QString::number(Y));
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint p = event->pos(); //Cursor position
    int X=p.x()-ui->Video->x();
    int Y=p.y()-ui->Video->y();
    QString str = "(" + QString::number(X) + "," + QString::number(Y) + ")";
    if (event->button() == Qt::LeftButton)
    {
        ui->label_statue->setText("Left Button Pressed");
        ui->label_X->setText("X: " + QString::number(X));
        ui->label_Y->setText("Y: " + QString::number(Y));
    }
    else if (event->button() == Qt::RightButton)
    {
        ui->label_statue->setText("Right Button Pressed");
        ui->label_X->setText("X: " + QString::number(X));
        ui->label_Y->setText("Y: " + QString::number(Y));
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event) // release
{
    QPoint p = event->pos();
    int X=p.x()-ui->Video->x();
    int Y=p.y()-ui->Video->y();
    ui->label_statue->setText("Mouse Released");
    ui->label_X->setText("X: " + QString::number(X));
    ui->label_Y->setText("Y: " + QString::number(Y));
    if(ImageArray.size()>0)
    {
        if (ImageArray[CurrentId]->qsUrl.size() != 0)
        {
            int X=p.x()-ui->Video->x();
            int Y=p.y()-ui->Video->y();
            for(auto link:ImageArray[CurrentId]->qsUrl){
                int left = link.second[1]; //X
                int right = link.second[3];
                int top = link.second[2]; //Y
                int bottom = link.second[4];
//                qDebug() << "left:" << left;
//                qDebug() << "right:" << right;
//                qDebug() << "top:" << top;
//                qDebug() << "bottom:" << bottom;
                if ((X > left) && (X < right) && (Y > top) && (Y < bottom))
                {
                    //LoadImage
                    ui->label_statue->setText("Next Video");
                    Timerswitch->stop();
                    SoundPlayer->stop();
                    LoadSecond(link.first,link.second[0]);
                }
            }
        }
    }
}
void MainWindow::Clear(){
    ImageArray.clear();
    JsonArray.clear();
    CurrentId=0;
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
        QStringList jsonfilter;
        jsonfilter << "*.json";
        data_path->setNameFilters(jsonfilter);
        QList<QFileInfo> *datafile = new QList<QFileInfo>(data_path->entryInfoList(jsonfilter));
        int datafileCount = datafile->count();
        if(datafileCount>0){
            QString jsonPath = datafile->at(0).filePath();
            getSecondData(jsonPath);
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
        audioOutput->setVolume(50);
        ui->PlayButton->setEnabled(false);
        ui->PauseButton->setEnabled(true);
        ui->RestartButton->setEnabled(true);
        //connect(SoundPlayer, &QMediaPlayer::durationChanged, this, [&](qint64 dur) { qDebug() << "duration = " << dur; });
        QObject::connect(SoundPlayer, &QMediaPlayer::mediaStatusChanged,
                             [&](QMediaPlayer::MediaStatus status){
                if(status == QMediaPlayer::LoadedMedia) {
                    statusBar()->showMessage(tr("Done"));
                    ShowImage();
                    SoundPlayer->setPosition(SoundPlayer->duration()/9000*CurrentId);
                    SoundPlayer->play();
                    Timerswitch->start(m_waitingtime);
                }
        });
    }
}

void MainWindow::on_RestartButton_clicked()
{
    CurrentId=0;
    Timerswitch->stop();
    SoundPlayer->stop();
    ui->PlayButton->setEnabled(true);
    ui->PauseButton->setEnabled(false);
    statusBar()->showMessage(tr("RESTART"));
    ShowImage();
}

