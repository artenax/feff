#include "form1.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>

form1::form1(int argc,char** argv) :    QWidget(){
    ui.setupUi(this);
    //this->setFixedSize(this->width(),this->height());

    QRect desk(QApplication::desktop()->availableGeometry(QApplication::desktop()->screenNumber(this)));
    move((desk.width() - width()) / 2, (desk.height() - height()) / 2);

    // date start 25.03.2011
    this->setWindowTitle("FeFF  v1.11 (13.06.2019)");
    this->setWindowIcon(QIcon(":feff.ico"));

    QString pathConfig = "profiles.ini";

    if (QFile::exists(pathConfig)==false){
        pathConfig=qgetenv("HOME")+"/.feff/profiles.ini";

        QDir dirConfig(qgetenv("HOME")+"/.feff/");
        if (dirConfig.exists()==false) dirConfig.mkpath(qgetenv("HOME")+"/.feff/");
    }


    confProfiles = new QSettings(pathConfig, QSettings::IniFormat);
    confProfiles->setPath(QSettings::IniFormat, QSettings::UserScope, QDir::currentPath());

    parsCommandLine( argc, argv);



    convert = new cConvert();

    pathFFmpeg="";
    ui.tabWidget->setTabEnabled(0,0);
    ui.tabWidget->setTabEnabled(1,0);
    ui.tabWidget->setTabEnabled(2,0);
    ui.tabWidget->setTabEnabled(3,0);
    ui.pushButton_5->setEnabled(false);

    ui.pushButton_2->setEnabled(0);
    ui.pushButton_3->setEnabled(0);


    //ui.groupBox_4->setVisible(0);

    tabMessage = ui.tabWidget->widget(6);
    ui.tabWidget->removeTab(ui.tabWidget->indexOf(tabMessage));

    if (tr("name_language")!="name_language"){
        ui.textEdit_2->insertPlainText(tr("Language:")+" "+tr("name_language")+"\n");
        ui.textEdit_2->insertPlainText(tr("Translator:")+" "+tr("name_translator")+"\n");
    }
    // tab debug
    if (debug!=1) ui.tabWidget->removeTab(5);


    btnConvert=0;

    ui.label_44->setText("Copyright DanSoft. All rights reserved.");

    // пока не поддерживается
    ui.label_10->setVisible(0);
    ui.label_11->setVisible(0);
    ui.label_12->setVisible(0);
    ui.label_13->setVisible(0);


    setFFmpeg("ffmpeg");
    if (infoFFmpeg.getTypeCoder() == ""){
        setFFmpeg("avconv");
    }
    ui.tabWidget->setCurrentIndex(0);

    connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(openFFmpeg()));
    connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(openSource()));
    connect(ui.pushButton_3,SIGNAL(clicked()),this,SLOT(openTarget()));
    connect(ui.pushButton_4,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui.pushButton_6,SIGNAL(clicked()),this,SLOT(profile_save()));
    connect(ui.pushButton_7,SIGNAL(clicked()),this,SLOT(profile_delete()));
    connect(ui.pushButton_8,SIGNAL(clicked()),this,SLOT(showFormInfo()));

    connect(ui.pushButton_5,SIGNAL(clicked()),this,SLOT(Click_pushButton_5()));
    connect(convert,SIGNAL(changeStatusConvert(sStatusConvert)),this,SLOT(convert_changeStatusConvert(sStatusConvert)));
    connect(convert,SIGNAL(readyData(QString)),this,SLOT(convert_readyData(QString)));
    connect(convert,SIGNAL(finish(int)),this,SLOT(ffmpeg_finish(int)));
    connect(convert,SIGNAL(error(QString)),this,SLOT(ffmpeg_error(QString)));
    connect(ui.comboBox_13,SIGNAL(currentIndexChanged(QString)),this,SLOT(profile_changed(QString)));

    connect(ui.comboBox_6,SIGNAL(currentIndexChanged(int)),this,SLOT(formatEdit(int)));


    // TODO
    // Feff is a Audio/Video converter for Linux using Qt4 as GUI and FFmpeg as backend.



    ui.textEdit_3->insertHtml(
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>"
        +tr(
        "1. COPYRIGHT, DISCLAIMER <BR>"
        "  (C) 1996-2019 AdmSasha <dik@inbox.ru> <BR>"
        "  This program is distributed in the hope that it will be useful,"
        "  but WITHOUT ANY WARRANTY; without even the implied warranty of "
        "  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<BR>"
        "<BR>"
        )+tr(
        "2. CONTACT<BR>"
        "  Please send bugreports to: <a href=mailto://dik@inbox.ru>dik@inbox.ru</a><BR>"
        "  Only bug reports with a reproducable bug are accepted.<BR>"
        "<BR>"
        )+tr(
        "3. THANKS<BR>"
        "  Elbert Pol <a href=mailto://elbert.pol@gmail.com>elbert.pol@gmail.com</a> <BR>"
        "  symbianflo <a href=http://qt-apps.org/usermanager/search.php?username=symbianflo>qt-apps.org</a><BR>"
        "<BR>"
        )+tr(
        "4. LICENSE<BR>"
        "  The program is distributed under the GPLv3<BR>"
        "<BR>"
        )
        +tr("5. DONATE <BR>")+
        " <a href=\"http://dansoft.krasnokamensk.ru/about.html\">http://dansoft.krasnokamensk.ru/about.html</a>"
        "<BR>"
        "</body></html>"
    );

    // FIXME
    ui.textEdit_3->verticalScrollBar()->setValue(1);

    ui.label_44->installEventFilter(this);

}

int form1::setFFmpeg(QString fileName){
    //
    //  Установка пути для FFmpeg
    //

    if (fileName == "") return 0;

    //infoFFmpeg.setProgramm(fileName);

    if (infoFFmpeg.setProgramm(fileName)==0){
        return 0;
    }

    if (infoFFmpeg.getVersion() == ""){
        return 0;
    }


    // получение версии FFmpeg
    ui.label_5->setText(infoFFmpeg.getVersion());

    // получение параметров сборки
    ui.textEdit->setText(infoFFmpeg.getConfiguration());


    // получение всех кодеков
    ffmpeg_codecs.clear();
    ffmpeg_codecs=infoFFmpeg.getCodecs();
    ui.label_7->setText(IntToStr(ffmpeg_codecs.size()));

    // получение всех форматов
    ffmpeg_formats.clear();
    ffmpeg_formats=infoFFmpeg.getFormats();
    ui.label_9->setText(IntToStr(ffmpeg_formats.size()));


    // заполнение полей кодека
    initFFmpegCodecs();

    // заполнение полей формата
    initFFmpegFormats();

    // заполнение полей Resolution
    initFFmpegResolution();

    // заполнение полей Aspect
    initFFmpegAspect();

    // заполнение полей Bitrate
    initFFmpegBitrate();

    // Заполнение полей Sampling rate
    initFFmpegSampling();

    // Заполнение поле Video Frame
    initFFmpegVideoFrame();

    // Заполнение поле Audio Channel
    initFFmpegChannels();


    //initProfiles
    initProfiles();
    ui.pushButton_7->setEnabled(0);
    profile_changed(ui.comboBox_13->itemText(0));

    ui.tabWidget->setTabEnabled(0,1);

    pathFFmpeg=fileName;
    ui.lineEdit->setText(pathFFmpeg);

    ui.pushButton_2->setEnabled(1);

    return 1;

}


int form1::setSource(QString fileName){
    int count_video=0;
    int count_audio=0;
    int count_subtitle=0;

    if (fileName == "") return 0;

    infoSource=infoFFmpeg.getInfoSource(fileName);

    ui.label_27->setText(infoSource.fileName);
    ui.label_23->setText(infoSource.duration);
    ui.label_25->setText(infoSource.format);
    ui.label_30->setText(infoSource.filesize);

    // создание дерева
    ui.treeWidget->clear();
    ui.comboBox_10->clear();

    QTreeWidgetItem *itemVideo = new QTreeWidgetItem(ui.treeWidget);
    QTreeWidgetItem *itemAudio = new QTreeWidgetItem(ui.treeWidget);
    QTreeWidgetItem *itemSubtitle = new QTreeWidgetItem(ui.treeWidget);

    ui.treeWidget->insertTopLevelItem(0,itemVideo);
    ui.treeWidget->insertTopLevelItem(0,itemAudio);
    ui.treeWidget->insertTopLevelItem(0,itemSubtitle);

    itemVideo->setText(0,tr("Video"));
    itemAudio->setText(0,tr("Audio"));
    itemSubtitle->setText(0,tr("Subtitle"));

    itemVideo->setExpanded(1);
    itemAudio->setExpanded(1);
    itemSubtitle->setExpanded(1);


    //заполнение дерева
    for (int i=0;i<16;i++){
        //  Video stream
        if (infoSource.streamVideo[i].number != ""){
            QTreeWidgetItem *__tempItemV = new QTreeWidgetItem(itemVideo);
            ui.treeWidget->insertTopLevelItem(0,__tempItemV);
            __tempItemV->setText(0,"("+infoSource.streamVideo[i].number + ")  " + infoSource.streamVideo[i].full);
            count_video++;
        }

        //  Audio stream
        if (infoSource.streamAudio[i].number != ""){
            QTreeWidgetItem *__tempItemA = new QTreeWidgetItem(itemAudio);
            ui.treeWidget->insertTopLevelItem(0,__tempItemA);
            __tempItemA->setText(0,"("+infoSource.streamAudio[i].number + ")  " + infoSource.streamAudio[i].full);
            count_audio++;
            ui.comboBox_10->addItem(infoSource.streamAudio[i].number);
        }

        //  Subtitle stream
        if (infoSource.streamSubtitle[i].number != ""){
            QTreeWidgetItem *__tempItemS = new QTreeWidgetItem(itemSubtitle);
            ui.treeWidget->insertTopLevelItem(0,__tempItemS);
            __tempItemS->setText(0,"("+infoSource.streamSubtitle[i].number + ")  " + infoSource.streamSubtitle[i].full);
            count_subtitle++;
        }

    }

    ui.groupBox->setChecked(1);
    ui.groupBox_2->setChecked(1);
    ui.groupBox_3->setChecked(1);


    // добавить (none), если нет stream
    if (count_video==0){
        QTreeWidgetItem *__tempItemV = new QTreeWidgetItem(itemVideo);
        ui.treeWidget->insertTopLevelItem(0,__tempItemV);
        __tempItemV->setText(0,"(none)");
        ui.groupBox->setChecked(0);
    }
    if (count_audio==0){
        QTreeWidgetItem *__tempItemA = new QTreeWidgetItem(itemAudio);
        ui.treeWidget->insertTopLevelItem(0,__tempItemA);
        __tempItemA->setText(0,"(none)");
        ui.groupBox_2->setChecked(0);
    }
    if (count_subtitle==0){
        QTreeWidgetItem *__tempItemS = new QTreeWidgetItem(itemSubtitle);
        ui.treeWidget->insertTopLevelItem(0,__tempItemS);
        __tempItemS->setText(0,"(none)");
        ui.groupBox_3->setChecked(0);
    }


    //Установить настройки по умолчанию для кодирования
    ////////////////////////////////////////////
    int ind=0;
    //////    VIDEO
/*
    if (count_video>0){
        // Установка Codec
        ind = ui.comboBox->findText(infoSource.streamVideo[1].codec);
        if (ind != -1){
            ui.comboBox->setCurrentIndex(ind);
        }else{
            ui.comboBox->setCurrentIndex(0);
        }

        // Установка Resolution
        ind = ui.comboBox_2->findText(infoSource.streamVideo[1].size.split(" ").at(0));
        if (ind != -1){
            ui.comboBox_2->setCurrentIndex(ind);
        }else{
            ui.comboBox_2->addItem(infoSource.streamVideo[1].size.split(" ").at(0));
            ui.comboBox_2->setCurrentIndex(ui.comboBox_2->count()-1);
        }

        // Установка bitrate
        ind = ui.comboBox_3->findText(infoSource.bitrate.split(" ").at(0));
        if (ind != -1){
            ui.comboBox_3->setCurrentIndex(ind);
        }else{
            ui.comboBox_3->addItem(infoSource.bitrate.split(" ").at(0));
            ui.comboBox_3->setCurrentIndex(ui.comboBox_3->count()-1);
        }

        // Установка Framerate
        if (infoSource.streamVideo[1].fps != ""){
            ind = ui.comboBox_11->findText(infoSource.streamVideo[1].fps.split(" ").at(0));
            if (ind != -1){
                ui.comboBox_11->setCurrentIndex(ind);
            }else{
                ui.comboBox_11->addItem(infoSource.streamVideo[1].fps.split(" ").at(0));
                ui.comboBox_11->setCurrentIndex(ui.comboBox_11->count()-1);
            }
        }
    }

    ////////////////////////////////////////////
    //////    AUDIO
    // Установка Codec
    ind = ui.comboBox_5->findText(infoSource.streamAudio[1].codec);
    if (ind != -1){
        ui.comboBox_5->setCurrentIndex(ind);
    }else{
        ui.comboBox_5->setCurrentIndex(0);
    }
*/
    // Установка Bitrate
    ind = ui.comboBox_4->findText(infoSource.streamAudio[1].bitrate.split(" ").at(0));
    if (ind != -1){
        ui.comboBox_4->setCurrentIndex(ind);
    }else{
        ui.comboBox_4->addItem(infoSource.streamAudio[1].bitrate.split(" ").at(0));
        ui.comboBox_4->setCurrentIndex(ui.comboBox_4->count()-1);
    }
/*
    // Установка SamplingRate
    ind = ui.comboBox_8->findText(infoSource.streamAudio[1].Sampling_rate);
    if (ind != -1){
        ui.comboBox_8->setCurrentIndex(ind);
    }else{
        ui.comboBox_8->addItem(infoSource.streamAudio[1].Sampling_rate);
        ui.comboBox_8->setCurrentIndex(ui.comboBox_8->count()-1);
    }
*/


    ui.pushButton_3->setEnabled(1);


    return 1;
}

void form1::openFFmpeg(){
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open FFmpeg"), "", tr("All Files (*)"));
    if (fileName.size() > 0){
        formWait("reading FFmpeg",1);
        if (setFFmpeg(fileName)==0){
            QMessageBox::information(this,tr("ffmpeg"),tr("Incorrect path to ffmpeg or ffmpeg does not support"));
            ui.lineEdit->setText("");
            ui.lineEdit_2->setText("");
            ui.lineEdit_3->setText("");
            ui.textEdit->setText("");
            ui.tabWidget->setTabEnabled(1,0);
            ui.tabWidget->setTabEnabled(2,0);
            ui.tabWidget->setTabEnabled(3,0);
            ui.label_5->setText("");
            ui.label_7->setText("");
            ui.label_9->setText("");
            ui.label_11->setText("");
            ui.label_13->setText("");
        }
        formWait("",0);
        ui.tabWidget->setCurrentIndex(0);
    }

}

void form1::openTarget(){
    QString fileName = QFileDialog::getSaveFileName(this,tr("Open target"), "", tr("All Files (*.*)"));
    if (fileName.size() > 0){
        formWait("reading source file",1);

        if (fileName.split(".").size()==1){
            if (infoSource.streamVideo[1].full!=""){
                fileName+=".avi";
            }else{
                fileName+=".mp3";
            }
        }
        formWait("",0);
        ui.lineEdit_3->setText(fileName);
        ui.tabWidget->setTabEnabled(2,1);
        ui.tabWidget->setCurrentIndex(2);
        ui.pushButton_5->setEnabled(true);
    }
}


void form1::openSource(){
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open source file"), "", tr("All Files (*.*)"));
    if (fileName.size() > 0){
        formWait(tr("reading source file"),1);

        ui.lineEdit_2->setText(fileName);

        setSource(fileName);
        formWait("",0);
        ui.tabWidget->setTabEnabled(1,1);
        ui.tabWidget->setCurrentIndex(1);
    }

}

void form1::Click_pushButton_5(){
    if (btnConvert==0){
        // запустить конвертирование
        ui.label_34->setText(0);
        ui.label_36->setText(0 );
        ui.label_38->setText(0);
        ui.label_40->setText(0);
        ui.label_42->setText(0);

        ui.progressBar->setValue(0);

        Convert();
        return;
    }

    if (btnConvert==1){
        // остановить конвертирование
        convert->stop();
        ui.tabWidget->setTabEnabled(3,0);
        ui.tabWidget->setCurrentIndex(2);
        formWait("",false);
        ui.pushButton_5->setText(tr("Convert"));
        btnConvert=0;
        return;

    }

    if (btnConvert==2){
        ui.tabWidget->setTabEnabled(1,0);
        ui.tabWidget->setTabEnabled(2,0);
        ui.tabWidget->setTabEnabled(3,0);
        ui.tabWidget->setCurrentIndex(0);
        ui.pushButton_3->setEnabled(0);
        ui.lineEdit_2->setText("");
        ui.lineEdit_3->setText("");
        ui.pushButton_5->setEnabled(false);
        formWait("",false);

        ui.pushButton_5->setText(tr("Convert"));
        btnConvert=0;
    }

}

void form1::Convert(){
    // Конвертирование
    QStringList arg;


    //Проверка правильно введенных параметров
    if (ui.comboBox_2->currentText() != ""){
        if (regex("^[0-9]{1,4}x[0-9]{1,4}$",ui.comboBox_2->currentText())==false){
            QMessageBox::information(this,tr("ffmpeg"),tr("Incorrect param for resolution"));
            return;
        }
    }
    if(ui.comboBox_3->currentText() !=""){
        if (regex("^\\d+$",ui.comboBox_3->currentText())==false){
            QMessageBox::information(this,tr("ffmpeg"),tr("Incorrect param for bitrate"));
            return;
        }
    }
    if (ui.comboBox_9->currentText() != ""){
        if (regex("^\\d+:\\d+$",ui.comboBox_9->currentText())==false){
            QMessageBox::information(this,tr("ffmpeg"),tr("Incorrect param for aspect ratio"));
            return;
        }
    }
    if (ui.comboBox_11->currentText() != ""){
        if (regex("^\\d+$",ui.comboBox_11->currentText())==false){
            QMessageBox::information(this,tr("ffmpeg"),tr("Incorrect param for frame rate"));
            return;
        }
    }
    if(ui.comboBox_4->currentText() !=""){
        if (regex("^\\d+$",ui.comboBox_4->currentText())==false){
            QMessageBox::information(this,tr("ffmpeg"),tr("Incorrect param for bitrate"));
            return;
        }
    }
    if(ui.comboBox_8->currentText() !=""){
        if (regex("^\\d+$",ui.comboBox_8->currentText())==false){
            QMessageBox::information(this,tr("ffmpeg"),tr("Incorrect param for sampling rate"));
            return;
        }
    }




    ui.tabWidget->setTabEnabled(3,1);
    ui.tabWidget->setCurrentIndex(3);

    ui.progressBar->setValue(0);


    //подготовка аргументов конвертирования
    arg << "-i" << ui.lineEdit_2->text();
    arg << "-y";

    // Video
    if (ui.groupBox->isChecked()==true){
        if (ui.comboBox->currentText() != "") arg << "-vcodec" << ui.comboBox->currentText();
        // -s size             set frame size (WxH or abbreviation)
        if (ui.comboBox_2->currentText() != "") arg << "-s" << ui.comboBox_2->currentText();
        // -vb bitrate         set bitrate (in bits/s)
        if (ui.comboBox_3->currentText() != "") arg << "-vb" << IntToStr(ui.comboBox_3->currentText().toInt()*1000);
        // -aspect aspect      set aspect ratio (4:3, 16:9 or 1.3333, 1.7777)
        if (ui.comboBox_9->currentText() != "") arg << "-aspect" << ui.comboBox_9->currentText();
        // -r rate             set frame rate (Hz value, fraction or abbreviation)
        if (ui.comboBox_11->currentText() != "") arg << "-r" << ui.comboBox_11->currentText();

    }else{
        arg << "-vn";
    }

    ///////////////
    // Audio
    /////////////
    if (ui.groupBox_2->isChecked()==true){
        // -acodec codec       force audio codec ('copy' to copy stream)
        if (ui.comboBox_5->currentText() != "") arg << "-acodec" << ui.comboBox_5->currentText();
        // -ab                <int>   E..A. set bitrate (in bits/s)
        if (ui.comboBox_4->currentText() != "") arg << "-ab" << DoubleToStr(ui.comboBox_4->currentText().toDouble()*1000);
        // -ar rate            set audio sampling rate (in Hz)
        if (ui.comboBox_8->currentText() != "") arg << "-ar" << ui.comboBox_8->currentText();
        //-ac channels        set number of audio channels
        if (ui.comboBox_12->currentText() != "") arg << "-ac" << ui.comboBox_12->itemData(ui.comboBox_12->currentIndex()).toString();
        arg << "-vol" << ui.spinBox->text();
    }else{
        arg << "-an";
    }

    // Subtitle
    if (ui.groupBox_3->isChecked()==true){
        if (ui.comboBox_7->currentText() != "") arg << "-scodec" << ui.comboBox_7->currentText();
    }else{
        arg << "-sn";
    }


    // подключение -map
    if (infoFFmpeg.getTypeCoder() == "ffmpeg"){
        if (ui.groupBox->isChecked()==true){
            arg << "-map" << infoSource.streamVideo[1].number.mid(8,3);
        }
        if (ui.groupBox_2->isChecked()==true){
            arg << "-map" << ui.comboBox_10->currentText().mid(8,3);
        }
    }

    // format
    if (ui.comboBox_6->currentText() != "") arg << "-f" << ui.comboBox_6->currentText();

    // pass
    if (ui.comboBox_14->currentText() != "") arg << "-pass" << ui.comboBox_14->currentText();


    // custom options
    QStringList customOptions= ui.lineEdit_4->text().split(" ");
    for (int i=0;i<customOptions.size();i++){
        if (ui.lineEdit_4->text() != "")  arg <<  customOptions[i];
    }



    // Файл target
    arg << ui.lineEdit_3->text();


    ui.textEdit_2->insertPlainText(arg.join(" "));


    convert->start(pathFFmpeg,arg);

    ui.pushButton_5->setText(tr("Stop"));
    btnConvert=1;
}


void form1::formWait(QString message,bool visible){

    if (visible==0){
        //ui.groupBox_4->lower();
        //ui.groupBox_4->setVisible(0);
        ui.tabWidget->removeTab(ui.tabWidget->indexOf(tabMessage));
    }

    if (visible==1){
        //ui.groupBox_4->setVisible(1);
        //ui.groupBox_4->raise();
        ui.tabWidget->insertTab(6,tabMessage,tr("Message"));
        ui.tabWidget->setCurrentIndex(ui.tabWidget->indexOf(tabMessage));
        ui.label_32->setText(message);
    }

}


void form1::convert_changeStatusConvert(sStatusConvert statusConvert){

    ui.label_34->setText(statusConvert.frame);
    ui.label_36->setText(statusConvert.fps );
    ui.label_38->setText(statusConvert.size);
    ui.label_40->setText(statusConvert.time);
    ui.label_42->setText(statusConvert.bitrate);

    ui.progressBar->setValue(StrToDouble( statusConvert.time)*100/convTime(infoSource.duration));

}

void form1::convert_readyData(QString text){
    //qDebug() << text;
    ui.textEdit_2->insertPlainText(text+"\n");
}


double form1::convTime(QString duration){
    //  конвертирование из Duration: 00:39:31.49, в число
    double time;

    QStringList clock=duration.split(".");
    QStringList h=clock[0].split(":");

    time = (StrToDouble(h[0])*3600)+(StrToDouble(h[1])*60)+(StrToDouble(h[2]));
    time+= (StrToDouble(clock[1])/100);

    return time;
}

void form1::ffmpeg_finish(int value){
    formWait(tr("Conversion completed"),true);
    ui.pushButton_5->setText(tr("Again"));
    btnConvert=2;

}

void form1::ffmpeg_error(QString err_str){
    formWait(err_str,true);
    ui.pushButton_5->setText(tr("Again"));
    btnConvert=1;
}

void form1::initFFmpegCodecs(){
    // Заполнение полей кодеков

    ui.comboBox->clear();
    ui.comboBox_5->clear();
    ui.comboBox_7->clear();

    // не применять
    ui.comboBox->addItem("","");
    ui.comboBox_5->addItem("","");
    ui.comboBox_7->addItem("","");

    // копирование
    ui.comboBox->addItem("copy","");
    ui.comboBox_5->addItem("copy","");
    ui.comboBox_7->addItem("copy","");

    // поддерживаемые
    QMapIterator<int, sInfoCodec> i(ffmpeg_codecs);
    while (i.hasNext()){
        i.next();
        if (i.value().encoding==true){
            if (i.value().typeCodec==1)  ui.comboBox->addItem(i.value().name ,"");
            if (i.value().typeCodec==2)  ui.comboBox_5->addItem(i.value().name ,"");
            if (i.value().typeCodec==3)  ui.comboBox_7->addItem(i.value().name ,"");
        }
    }

}

void form1::initFFmpegFormats(){
    ui.comboBox_6->clear();

    ui.comboBox_6->addItem("","");

    // поддерживаемые
    QMapIterator<int, sInfoFormat> i(ffmpeg_formats);
    while (i.hasNext()){
        i.next();
        if (i.value().muxing==true){
            ui.comboBox_6->addItem(i.value().name ,"");
        }
    }

}

void form1::initFFmpegResolution(){
    ui.comboBox_2->clear();

    ui.comboBox_2->addItem("","");

    ui.comboBox_2->addItem("160x128","");
    ui.comboBox_2->addItem("176x144","");
    ui.comboBox_2->addItem("224x168","");
    ui.comboBox_2->addItem("224x176","");
    ui.comboBox_2->addItem("320x240","");
    ui.comboBox_2->addItem("400x300","");
    ui.comboBox_2->addItem("480x320","");
    ui.comboBox_2->addItem("512x384","");
    ui.comboBox_2->addItem("640x480","");
    ui.comboBox_2->addItem("624x352","");
    ui.comboBox_2->addItem("700x342","");
    ui.comboBox_2->addItem("720x400","");
    ui.comboBox_2->addItem("720x544","");
    ui.comboBox_2->addItem("800x600","");
    ui.comboBox_2->addItem("1024x768","");
    ui.comboBox_2->addItem("1920x1080","");

}


void form1::initFFmpegAspect(){
    ui.comboBox_9->clear();

    ui.comboBox_9->addItem("","");
    ui.comboBox_9->addItem("4:3","");
    ui.comboBox_9->addItem("16:9","");
    ui.comboBox_9->addItem("37:20","");
    ui.comboBox_9->addItem("47:20","");
}


void form1::initFFmpegBitrate(){

    // Video
    ui.comboBox_3->clear();

    ui.comboBox_3->addItem("","");
    ui.comboBox_3->addItem("32","");
    ui.comboBox_3->addItem("64","");
    ui.comboBox_3->addItem("128","");
    ui.comboBox_3->addItem("256","");
    ui.comboBox_3->addItem("512","");
    ui.comboBox_3->addItem("768","");
    ui.comboBox_3->addItem("1024","");
    ui.comboBox_4->addItem("1536","");
    ui.comboBox_3->addItem("2048","");
    ui.comboBox_3->addItem("3072","");
    ui.comboBox_3->addItem("4096","");
    ui.comboBox_3->addItem("5120","");
    ui.comboBox_3->addItem("6144","");

    // Audio
    ui.comboBox_4->clear();

    ui.comboBox_4->addItem("","");
    ui.comboBox_4->addItem("32","");
    ui.comboBox_4->addItem("64","");
    ui.comboBox_4->addItem("128","");
    ui.comboBox_4->addItem("256","");
    ui.comboBox_4->addItem("512","");
    ui.comboBox_4->addItem("768","");
    ui.comboBox_4->addItem("1024","");
    ui.comboBox_4->addItem("1536","");
    ui.comboBox_4->addItem("2048","");
    ui.comboBox_4->addItem("3072","");
    ui.comboBox_4->addItem("4096","");
    ui.comboBox_4->addItem("5120","");
    ui.comboBox_4->addItem("6144","");

}

void form1::initFFmpegSampling(){
    ui.comboBox_8->clear();

    ui.comboBox_8->addItem("","");
    ui.comboBox_8->addItem("8000","");
    ui.comboBox_8->addItem("11025","");
    ui.comboBox_8->addItem("22050","");
    ui.comboBox_8->addItem("44100","");
    ui.comboBox_8->addItem("48000","");
}

void form1::initFFmpegVideoFrame(){
    ui.comboBox_11->clear();

    ui.comboBox_11->addItem("","");
    ui.comboBox_11->addItem("10","");
    ui.comboBox_11->addItem("15","");
    ui.comboBox_11->addItem("20","");
    ui.comboBox_11->addItem("24","");
    ui.comboBox_11->addItem("25","");
}

void form1::initFFmpegChannels(){
    ui.comboBox_12->clear();

    ui.comboBox_12->addItem("","");
    ui.comboBox_12->addItem("Mono","1");
    ui.comboBox_12->addItem("Sterio","2");
    ui.comboBox_12->addItem("5.1","6");
}



void form1::initProfiles(){
    QStringList groups = confProfiles->childGroups();

    ui.comboBox_13->clear();
    ui.comboBox_13->addItem("Default","");

    for (int i=0;i<groups.size();i++){
        if (groups[i] != "Default"){
            ui.comboBox_13->addItem(groups[i],"");
        }
    }
}


void form1::profile_changed(QString key){
    int ind;
    if (key == ""){
        ui.pushButton_7->setEnabled(0);
        return ;
    }else{
        ui.pushButton_7->setEnabled(1);
    }


    // Установка format
    ind = ui.comboBox_6->findText(confProfiles->value(key+"/public/format/","").toString());
    if (ind != -1){
        ui.comboBox_6->setCurrentIndex(ind);
    }else{
        ui.comboBox_6->addItem(confProfiles->value(key+"/public/format/","").toString(),"");
        ui.comboBox_6->setCurrentIndex(ui.comboBox_6->count()-1);
    }
    // Установка video
    ui.groupBox->setChecked(confProfiles->value(key+"/public/video/","true").toBool() );
    // Установка audio
    ui.groupBox_2->setChecked(confProfiles->value(key+"/public/audio/","true").toBool());
    // Установка subtitle
    ui.groupBox_3->setChecked(confProfiles->value(key+"/public/subtitle/","").toBool());

    // установка pass
    ind = ui.comboBox_14->findText(confProfiles->value(key+"/public/pass/","").toString());
    if (ind != -1){
        ui.comboBox_14->setCurrentIndex(ind);
    }
    // установка custom options
    ui.lineEdit_4->setText(confProfiles->value(key+"/public/customOptions/","").toString() );



    ////////////////////////////////////////////
    //////    VIDEO
    // Установка Codec
    ind = ui.comboBox->findText(confProfiles->value(key+"/Video/Codec/","").toString());
    if (ind != -1){
        ui.comboBox->setCurrentIndex(ind);
    }else{
        ui.comboBox->setCurrentIndex(0);
    }

    // Установка Resolution
    ind = ui.comboBox_2->findText(confProfiles->value(key+"/Video/resolution/","").toString());
    if (ind != -1){
        ui.comboBox_2->setCurrentIndex(ind);
    }else{
        ui.comboBox_2->addItem(confProfiles->value(key+"/Video/resolution/","").toString(),"");
        ui.comboBox_2->setCurrentIndex(ui.comboBox_2->count()-1);
    }

    // Установка bitrate
    ind = ui.comboBox_3->findText(confProfiles->value(key+"/Video/bitrate/","").toString());
    if (ind != -1){
        ui.comboBox_3->setCurrentIndex(ind);
    }else{
        ui.comboBox_3->addItem(confProfiles->value(key+"/Video/bitrate/","").toString(),"");
        ui.comboBox_3->setCurrentIndex(ui.comboBox_3->count()-1);
    }

    // Установка AspectRatio
    ind = ui.comboBox_9->findText(confProfiles->value(key+"/Video/AspectRatio/","").toString());
    if (ind != -1){
        ui.comboBox_9->setCurrentIndex(ind);
    }else{
        ui.comboBox_9->addItem(confProfiles->value(key+"/Video/AspectRatio/","").toString(),"");
        ui.comboBox_9->setCurrentIndex(ui.comboBox_9->count()-1);
    }

    // Установка Framerate
    ind = ui.comboBox_11->findText(confProfiles->value(key+"/Video/FrameRate/","").toString());
    if (ind != -1){
        ui.comboBox_11->setCurrentIndex(ind);
    }else{
        ui.comboBox_11->addItem(confProfiles->value(key+"/Video/FrameRate/","").toString(),"");
        ui.comboBox_11->setCurrentIndex(ui.comboBox_11->count()-1);
    }


    ////////////////////////////////////////////
    //////    AUDIO
    // Установка Codec
    ind = ui.comboBox_5->findText(confProfiles->value(key+"/audio/codec/","").toString());
    if (ind != -1){
        ui.comboBox_5->setCurrentIndex(ind);
    }else{
        ui.comboBox_5->setCurrentIndex(0);
    }

    // Установка Bitrate
    ind = ui.comboBox_4->findText(confProfiles->value(key+"/audio/Bitrate/","").toString());
    if (ind != -1){
        ui.comboBox_4->setCurrentIndex(ind);
    }else{
        ui.comboBox_4->addItem(confProfiles->value(key+"/audio/Bitrate/","").toString(),"");
        ui.comboBox_4->setCurrentIndex(ui.comboBox_4->count()-1);
    }

    // Установка SamplingRate
    ind = ui.comboBox_8->findText(confProfiles->value(key+"/audio/SamplingRate/","").toString());
    if (ind != -1){
        ui.comboBox_8->setCurrentIndex(ind);
    }else{
        ui.comboBox_8->addItem(confProfiles->value(key+"/audio/SamplingRate/","").toString(),"");
        ui.comboBox_8->setCurrentIndex(ui.comboBox_8->count()-1);
    }

    // Установка Channels
    ind = ui.comboBox_12->findText(confProfiles->value(key+"/audio/Channels/","").toString());
    if (ind != -1){
        ui.comboBox_12->setCurrentIndex(ind);
    }else{
        ui.comboBox_12->setCurrentIndex(0);
    }

    // установка volume
    ui.spinBox->setValue(confProfiles->value(key+"/audio/volume/","256").toInt());


    ////////////////////////////////////////////
    //////    SUBTITLE
    // Установка Codec
    ind = ui.comboBox_7->findText(confProfiles->value(key+"/subtitle/codec/","").toString());
    if (ind != -1){
        ui.comboBox_7->setCurrentIndex(ind);
    }else{
        ui.comboBox_7->addItem(confProfiles->value(key+"/subtitle/codec/","").toString(),"");
        ui.comboBox_7->setCurrentIndex(0);
    }



}



void form1::profile_save(){
    //   Сохранение профайла


    formSave form;
    form.setProfileName(ui.comboBox_13->currentText());
    form.exec();
    QString profileName = form.getProfileName();
    if (profileName != ""){
        confProfiles->setValue(profileName+"/public/format", ui.comboBox_6->currentText());
        confProfiles->setValue(profileName+"/public/video", ui.groupBox->isChecked());
        confProfiles->setValue(profileName+"/public/audio", ui.groupBox_2->isChecked());
        confProfiles->setValue(profileName+"/public/subtitle", ui.groupBox_3->isChecked());
        confProfiles->setValue(profileName+"/public/pass", ui.comboBox_14->currentText());
        confProfiles->setValue(profileName+"/public/customOptions", ui.lineEdit_4->text());
        //
        confProfiles->setValue(profileName+"/video/codec", ui.comboBox->currentText());
        confProfiles->setValue(profileName+"/video/Resolution", ui.comboBox_2->currentText());
        confProfiles->setValue(profileName+"/video/Bitrate", ui.comboBox_3->currentText());
        confProfiles->setValue(profileName+"/video/AspectRatio", ui.comboBox_9->currentText());
        confProfiles->setValue(profileName+"/video/FrameRate", ui.comboBox_11->currentText());
        //
        confProfiles->setValue(profileName+"/audio/codec", ui.comboBox_5->currentText());
        confProfiles->setValue(profileName+"/audio/Bitrate", ui.comboBox_4->currentText());
        confProfiles->setValue(profileName+"/audio/SamplingRate", ui.comboBox_8->currentText());
        confProfiles->setValue(profileName+"/audio/Channels", ui.comboBox_12->currentText());
        confProfiles->setValue(profileName+"/audio/volume", ui.spinBox->text());
        //
        confProfiles->setValue(profileName+"/subtitle/codec", ui.comboBox_7->currentText());
        //

        //

        initProfiles();
        ui.comboBox_13->setCurrentIndex(ui.comboBox_13->findText(profileName));
    }
    form.deleteLater();

}

void form1::profile_delete(){
    //   Удаление профайла

    if (QMessageBox::question(this,"Удаление профайла","Вы уверены, что хотите удалить этот профиль ?",QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes){
        confProfiles->remove(ui.comboBox_13->currentText());
        initProfiles();
    }
  

}

//parsing the command line
void form1::parsCommandLine(int argc,char** argv){

    debug=0;

    for (int i=0;i<argc;i++){
        if (trim(argv[i]) == "-debug") debug=1;
    }


}



bool form1::eventFilter(QObject *o, QEvent *e){

    if (e->type() == QEvent::MouseButtonRelease){
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
        if (mouseEvent->button() == Qt::LeftButton){
            if (o == ui.label_44){
                QDesktopServices::openUrl(QUrl("http://dansoft.krasnokamensk.ru"));
                return 1;
            }
        }
    }

    return QWidget::eventFilter(o, e);
}


void form1::showFormInfo(){
    infoSource=infoFFmpeg.getInfoSource(ui.lineEdit_2->text());

    formInfo form(0,infoSource);
    form.exec();
    form.deleteLater();
}

void form1::formatEdit(int index){
    if (autoEditExtension==false) return;

    QString format = ui.comboBox_6->currentText();
    QString oldFormat = ui.lineEdit_3->text().split(".").takeLast();

    QString fileName =  ui.lineEdit_3->text().mid(0,ui.lineEdit_3->text().length()-oldFormat.length()-1)+"."+format;

    ui.lineEdit_3->setText(fileName);
}
