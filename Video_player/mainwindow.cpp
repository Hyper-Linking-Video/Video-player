#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QMouseEvent>
#include <QAudioOutput>
//#include <QFile>
#include <QTimer>

MyImage VideoFrame[9000];

//load json first
//create MainImage, for each second video infomation, add the url to image(index)
//add the path to image(index)
//
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
    labelStatus = new QLabel();
    labelStatus->setMinimumSize(300, 20);
    labelStatus->setFrameShape(QFrame::WinPanel);
    labelStatus->setFrameShadow(QFrame::Sunken);
    labelStatus->setText(tr("Mouse Position:"));
    labelStatus->setFixedWidth(100);
    labelMousePos = new QLabel();
    labelMousePos->setText(tr(""));
    labelMousePos->setFixedWidth(100);
    statusBar()->addPermanentWidget(labelStatus);
    statusBar()->addPermanentWidget(labelMousePos);
    this->setMouseTracking(true);
    connect(this->ui->net_set, SIGNAL(clicked(bool)), this, SLOT(mouseReleaseEvent(QMouseEvent * e)));
    //image
    //m_waitingtime=;//based on the sound
    m_currentindex = 0;
    m_timerswitch = new QTimer(this);
    m_timerswitch->setSingleShot(true);
    //QTimer::singleShot(m_waitingtime, this,SLOT(OnTimerSwitch()));
    connect(m_timerswitch, SIGNAL(timeout()), this, SLOT(OnTimerSwitch()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
bool MainWindow::getSecondData(QScatterSeries *series)
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

    if (echo_info.contains("echo_information") && echo_info["echo_information"].isArray())
    {
        QJsonArray info_array = echo_info["echo_information"].toArray();
        for (int i = 0; i < info_array.size(); i++)
        {
            QJsonObject item_object = info_array[i].toObject();

            qreal AimVideoFrameNumber = -1;
            qreal BottomRightPointX = -1;
            qreal BottomRightPointY = -1;
            qreal TopLeftPointX = -1;
            qreal TopLeftPointX = -1;
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
            if (BottomRightPointX >= TopLeftPointX || BottomRightPointY >= TopLeftPointY || PrimaryVideoFrameNumber < 0 || AimVideoFrameNumber < 0)
                series->append(AimVideoFrameNumber, PrimaryVideoFrameNumber, BottomRightPointX, BottomRightPointY, TopLeftPointX, TopLeftPointY);
            else
            {
                qWarning("data read error");
                return false;
            }
        }
    }

    return true;
}
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    QPoint LeftTopPoint = e->pos(); //Cursor position
    labelMousePos->setText("(" + QString::number(LeftTopPoint.x()) + "," + QString::number(LeftTopPoint.y()) + ")");
}
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    QString str = "(" + QString::number(e->x()) + "," + QString::number(e->y()) + ")";
    if (e->button() == Qt::LeftButton)
    {
        statusBar()->showMessage(tr("Mouse Left Button Pressed:") + str);
    }
    else if (e->button() == Qt::RightButton)
    {
        statusBar()->showMessage(tr("Mouse Right Button Pressed:") + str);
    }
    else if (e->button() == Qt::MidButton)
    {
        statusBar()->showMessage(tr("Mouse Middle Button Pressed:") + str);
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *e) / release
{
    QPoint Point = e->pos();
    int X = Point->x();
    int Y = Point->y();
    QString str = "(" + QString::number(e->x()) + "," + QString::number(e->y()) + ")";
    statusBar()->showMessage(tr("Mouser Released:") + str, 3000);
    if (m_imageArray[m_currentindex]->qsUrl != NULL)
    {
        // int left=;
        // int right=;
        // int top=;
        // int bottom=;
        if (X > left && X < right && Y < top && Y > bottom)
        {
            net_set_ui Second; //link found,jump to another one
            Second.exec();
        }
    }
}
int MainWindow::addImage(QString imagePath, QString url /*= NULL*/)
{
    MainImage *img = new MainImage;
    img->iId = g_imageid;
    g_imageid++;
    img->qsImagePath = imagePath;
    img->qsUrl = url;
    m_imageArray.push_back(img);
    return img->iId;
}
void ScrollImageWidget::startTimer()
{
    m_timerswitch->stop();
    m_timerswitch->start(m_waitingtime);
}
void MainWindow::OnTimerSwitch()
{
    QPixmap nextimg;
    m_currentindex++;
    if (m_currentindex >= m_imageArray.size())
    {
        //QImage image(data,width,height,每行字节数,QImage::Format_RGB888);
        //nextimg=QPixmap::fromImage(image);
        //ui->Video->setPixmap(QPixmap::fromImage(image));
        nextimg.load(m_imageArray[0]->qsImagePath);
    }
    else
    {
        nextimg.load(m_imageArray[m_currentindex]->qsImagePath);
    }
    this->startTimer();
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
    QAudioOutput *audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile("E://AIFilmOne//AIFilmOne.wav"));
    audioOutput->setVolume(50);
    player->play();
}

void MainWindow::setPosition(int position)
{
    Sound->setPosition(position);
}