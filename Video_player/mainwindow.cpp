#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QAudioOutput>

MyImage VideoFrame[9000];

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->PlayButton->setEnabled(false);
    ui->PauseButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_LoadVideoButton_clicked()
{
    QString dir_name = QFileDialog::getExistingDirectory(NULL,"Please choose the directory with the primary video's frame files",".");
        qDebug() << "Dir Path:" << dir_name;
        if (dir_name.size()>0)
        {
            QDir *inputDir = new QDir(dir_name);
            QStringList filter;
            filter<<"*.rgb";
            inputDir->setNameFilters(filter);
            QList<QFileInfo> *fileInfo=new QList<QFileInfo>(inputDir->entryInfoList(filter));
            int fileCount = fileInfo->count();
            qDebug() << fileCount;

            if(fileCount<9000){
                QMessageBox::warning(this,tr("Error!"),tr("The frame files in this directory are not enough to generate a video. Please choose another directory."));

                return;
            }

            for(int i = 0; i < fileCount;i++){
                VideoFrame[i].setWidth(352);
                VideoFrame[i].setHeight(288);

                QByteArray pathTemp = fileInfo->at(i).filePath().toLatin1();

                VideoFrame[i].setImagePath(pathTemp.data());
            }
            unsigned char* VideoFrameTemp = new unsigned char[3*352*288]();
            VideoFrame[0].ReadImage();
            VideoFrame[0].setFlag();
            VideoFrameTemp=(unsigned char*)VideoFrame[0].getImageData();
            QImage *tempImage=new QImage(VideoFrameTemp,352,288,QImage::Format_RGB888);
            ui->Video->setAlignment(Qt::AlignCenter);
            ui->Video->setPixmap(QPixmap::fromImage(*tempImage));
            ui->PlayButton->setEnabled(true);
        }
}


void MainWindow::on_PlayButton_clicked(bool checked)
{
    ui->PlayButton->setEnabled(false);
    ui->PauseButton->setEnabled(false);
    QMediaPlayer* player = new QMediaPlayer;
    QAudioOutput* audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile("E://AIFilmOne//AIFilmOne.wav"));
    audioOutput->setVolume(50);
    player->play();
}

