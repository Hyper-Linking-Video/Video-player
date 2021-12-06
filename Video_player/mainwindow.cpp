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

//MyImage VideoFrame[9000];

//load image(play video),play sound
//if click,check url,check cursor location
//if have url and in the boundaries, jump to the second video
//Q: 多线程？（button，image_time，audio，click_check);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Window1");
    ui->PlayButton->setEnabled(false);
    ui->PauseButton->setEnabled(false);
    //Cursor check
    setCursor(Qt::CrossCursor);
    ui->label_statue->setText("");
    ui->label_X->setText("X: ");
    ui->label_Y->setText("Y: ");
    this->setMouseTracking(true);
//    connect(this->ui->listWidget, SIGNAL(clicked(bool)), this, SLOT(mouseReleaseEvent(QMouseEvent * e)));
//    //image
//    m_currentindex = 0;
//    Timerswitch = new QTimer(this);
//    Timerswitch->setSingleShot(true);
//    //QTimer::singleShot(m_waitingtime, this,SLOT(OnTimerSwitch()));
//    connect(Timerswitch, SIGNAL(timeout()), this, SLOT(OnTimerSwitch()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->pos(); //Cursor position
    ui->label_X->setText("X: "+QString::number(p.x()-1));
    ui->label_Y->setText("Y: "+QString::number(p.y()-47));
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint p = event->pos(); //Cursor position
    QString str = "(" + QString::number(p.x()) + "," + QString::number(p.y()) + ")";
    if (event->button() == Qt::LeftButton)
    {
        ui->label_statue->setText("Left Button Pressed");
        ui->label_X->setText("X: "+QString::number(p.x()));
        ui->label_Y->setText("Y: "+QString::number(p.y()));
    }
    else if (event->button() == Qt::RightButton)
    {
        ui->label_statue->setText("Right Button Pressed");
        ui->label_X->setText("X: "+QString::number(p.x()));
        ui->label_Y->setText("Y: "+QString::number(p.y()));
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event) // release
{
    QPoint p=event->pos();
    int X = p.x();
    int Y = p.y();
    ui->label_statue->setText("Mouse Released");
    ui->label_X->setText("X: "+QString::number(X));
    ui->label_Y->setText("Y: "+QString::number(Y));
//    if (!ImageArray[m_currentindex]->qsUrl.isNull())
//    {
//        int left=1;
//        int right=1;
//        int top=1;
//        int bottom=1;
//        if (X > left && X < right && Y < top && Y > bottom)
//        {
//            //LoadImage
//            UpdateTimer();
//        }
//    }
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

            if (item_object.contains("AimVideoFrameNumber") && item_object["AimVideoFrameNumber"].toInt())
            {
                AimVideoFrameNumber = item_object["AimVideoFrameNumber"].toInt();
            }
            else if (item_object.contains("PrimaryVideoFrameNumber") && item_object["PrimaryVideoFrameNumber"].toInt())
            {
                PrimaryVideoFrameNumber = item_object["PrimaryVideoFrameNumber"].toInt();
            }
            else if (item_object.contains("BottomRightPointX") && item_object["BottomRightPointX"].toInt())
            {
                BottomRightPointX = item_object["BottomRightPointX"].toInt();
            }
            else if (item_object.contains("TopLeftPointX") && item_object["TopLeftPointX"].toInt())
            {
                TopLeftPointX = item_object["TopLeftPointX"].toInt();
            }
            else if (item_object.contains("TopLeftPointX") && item_object["TopLeftPointX"].toInt())
            {
                TopLeftPointY = item_object["TopLeftPointX"].toInt();
            }
            else if (item_object.contains("BottomRightPointY") && item_object["BottomRightPointY"].toInt())
            {
                BottomRightPointY = item_object["BottomRightPointY"].toInt();
            }
            if (BottomRightPointX >= TopLeftPointX || BottomRightPointY >= TopLeftPointY || PrimaryVideoFrameNumber < 0 || AimVideoFrameNumber < 0){
                SecondImage *img = new SecondImage;
                img->AimVideoFrameNumber=AimVideoFrameNumber;
                img->PrimaryVideoFrameNumber=PrimaryVideoFrameNumber;
                img->BottomRightPointX=BottomRightPointX;
                img->BottomRightPointY=BottomRightPointY;
                img->TopLeftPointX=TopLeftPointX;
                img->TopLeftPointY=TopLeftPointY;
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

void MainWindow::LoadImage(int imageid,QString imagePath)
{
    MainImage *img = new MainImage;
    img->iId = imageid;
    img->qsImagePath = imagePath;
    ImageArray[imageid]=img;
    return;
}
void MainWindow::AddLink(){
    for(auto info:JsonArray){
        auto index=std::find(ImageArray.begin(),ImageArray.end(),info->PrimaryVideoFrameNumber);
        if(index!=ImageArray.end()){
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl.push_back(info->AimVideoFrameNumber);
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl.push_back(info->TopLeftPointX);
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl.push_back(info->TopLeftPointY);
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl.push_back(info->BottomRightPointX);
            ImageArray[info->PrimaryVideoFrameNumber]->qsUrl.push_back(info->BottomRightPointY);
        }
    }
}
void MainWindow::UpdateTimer()
{
    Timerswitch->stop();
    Timerswitch->start(m_waitingtime);
}
void MainWindow::OnTimerSwitch()
{
    QPixmap nextimg;
    CurrentId++;
    if (CurrentId >= ImageArray.size())
    {
        //QImage image(data,width,height,每行字节数,QImage::Format_RGB888);
        //nextimg=QPixmap::fromImage(image);
        //ui->Video->setPixmap(QPixmap::fromImage(image));
        nextimg.load(ImageArray[0]->qsImagePath);
    }
    else
    {
        if(ui->PlayButton->isEnabled()){
            ShowImage();
        }
        nextimg.load(ImageArray[CurrentId]->qsImagePath);
    }
    this->UpdateTimer();
}

void MainWindow::on_LoadVideoButton_clicked()
{
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
        getSecondData(data_path);

        for (int i = 0; i < fileCount; i++)
        {
            QString imagePath = fileInfo->at(i).filePath();
            LoadImage(i,imagePath);
        }
        AddLink();
        ui->PlayButton->setEnabled(true);
    }
}
void MainWindow::ShowImage(){
    MyImage VideoFrame;
    VideoFrame.setWidth(352);
    VideoFrame.setHeight(288);
    auto pathTemp=ImageArray[CurrentId]->qsImagePath.toLatin1();
    VideoFrame.setImagePath(pathTemp.data());
    unsigned char *VideoFrameTemp = new unsigned char[3 * 352 * 288]();
    VideoFrame.ReadImage();
    VideoFrame.setFlag();
    VideoFrameTemp = (unsigned char *)VideoFrame.getImageData();
    QImage *tempImage = new QImage(VideoFrameTemp, 352, 288, QImage::Format_RGB888);
    ui->Video->setAlignment(Qt::AlignCenter);
    ui->Video->setPixmap(QPixmap::fromImage(*tempImage));
}
void MainWindow::on_PlayButton_clicked(bool checked)
{
    //ui->PlayButton->setEnabled(false);
    ShowImage();
    //audio
    SoundPlayer = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    SoundPlayer->setAudioOutput(audioOutput);
    QDir *videoPath = new QDir(VideoPath);
    QStringList wavfilter;
    wavfilter << "*.wav";
    videoPath->setNameFilters(wavfilter);
    QList<QFileInfo> *wavfile = new QList<QFileInfo>(videoPath->entryInfoList(wavfilter));
    QString filepath = wavfile->at(0).filePath();
    qDebug() << filepath;
    SoundPlayer->setSource(QUrl::fromLocalFile(wavfile->at(0).filePath()));

    audioOutput->setVolume(50);
    SoundPlayer->play();
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    audioOutput->setVolume(value);
}
/*
void MainWindow::on_LoadVideoButton_clicked()
{
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
            VideoFrame[i].setWidth(352);
            VideoFrame[i].setHeight(288);

            QByteArray pathTemp = fileInfo->at(i).filePath().toLatin1();

            VideoFrame[i].setImagePath(pathTemp.data());
        }
        unsigned char *VideoFrameTemp = new unsigned char[3 * 352 * 288]();
        VideoFrame[0].ReadImage();
        VideoFrame[0].setFlag();
        VideoFrameTemp = (unsigned char *)VideoFrame[0].getImageData();
        QImage *tempImage = new QImage(VideoFrameTemp, 352, 288, QImage::Format_RGB888);
        ui->Video->setAlignment(Qt::AlignCenter);
        ui->Video->setPixmap(QPixmap::fromImage(*tempImage));
        ui->PlayButton->setEnabled(true);
    }
}

void MainWindow::on_PlayButton_clicked(bool checked)
{
    ui->PlayButton->setEnabled(false);
    ui->PauseButton->setEnabled(false);
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    QDir *videoPath = new QDir(VideoPath);
    QStringList wavfilter;
    wavfilter << "*.wav";
    videoPath->setNameFilters(wavfilter);
    QList<QFileInfo> *wavfile = new QList<QFileInfo>(videoPath->entryInfoList(wavfilter));
    QString filepath = wavfile->at(0).filePath();
    qDebug() << filepath;
    player->setSource(QUrl::fromLocalFile(wavfile->at(0).filePath()));

    audioOutput->setVolume(50);
    player->play();
}
*/


