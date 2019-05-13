#include "formInfo.h"

formInfo::formInfo(QWidget *parent,sInfoSource infoSource) : QDialog(parent){
    ui.setupUi(this);
    this->setFixedSize(this->width(),this->height());


    this->setWindowTitle(tr("Media info"));
    this->setWindowIcon(QIcon(":feff.ico"));

    // Video
    ui.lineEdit->setText(infoSource.fileName);                  // Video Name
    ui.lineEdit_2->setText(infoSource.filePath);                // Video path
    ui.lineEdit_3->setText(infoSource.streamVideo[1].codec);    // Video codec
    ui.lineEdit_4->setText(infoSource.format);                  // Container
    ui.lineEdit_5->setText(infoSource.bitrate);                 // Bitrate
    ui.lineEdit_6->setText(infoSource.streamVideo[1].size);     // Size
    ui.lineEdit_7->setText(infoSource.streamVideo[1].fps);      // Framerate
    ui.lineEdit_8->setText(infoSource.duration);                // Duration

    //Audio
    ui.lineEdit_9->setText(infoSource.streamAudio[1].codec);             // Audio codec
    ui.lineEdit_10->setText(infoSource.streamAudio[1].bitrate);          // Bitrate
    ui.lineEdit_11->setText(infoSource.streamAudio[1].Sampling_rate);    // Sampling rate
    ui.lineEdit_12->setText(infoSource.streamAudio[1].channels);         // Channels

    connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(close()));

}