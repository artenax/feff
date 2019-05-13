#include "cConvert.h"

cConvert::cConvert(QWidget *parent) :    QWidget(parent){
    procFFmpeg  = new QProcess();

    // отключить цвет
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("FFMPEG_FORCE_NOCOLOR", "1");
    procFFmpeg->setProcessEnvironment(env);


    connect(procFFmpeg,SIGNAL(readyRead()),this,SLOT(ffmpeg_readyRead()));
    connect(procFFmpeg,SIGNAL(readyReadStandardError()),this,SLOT(ffmpeg_readyRead()));
    connect(procFFmpeg,SIGNAL(readyReadStandardOutput()),this,SLOT(ffmpeg_readyRead()));
    connect(procFFmpeg,SIGNAL(finished(int)),this,SLOT(ffmpeg_finish(int)));
}


void cConvert::start(QString pathFFmpeg,QStringList arg){
    dataOutput="";
    procFFmpeg->start(pathFFmpeg,arg);
}


void cConvert::ffmpeg_readyRead(){
    QString data="";

    data.append(dataOutput);
    data.append(procFFmpeg->readAllStandardError());
    data.append(procFFmpeg->readAllStandardOutput());

    QStringList list = data.split(QRegExp("\n|\r")) ;

    dataOutput=list[list.size()-1];

    //"size=     100kB time=12.77 bitrate=  64.0kbits/s"
    // frame=  972 fps=418 q=31.0 Lsize=    1650kB time=40.58 bitrate= 333.2kbits/s dup=0 drop=1
    //video:1256kB audio:321kB global headers:0kB muxing overhead 4.638438%
    //
    //    FFmpeg - git-N-28843-g2cae980
    //frame= 1083 fps=527 q=3.5 Lsize=    1972kB time=54.15 bitrate= 298.4kbits/s
    //
    // ffmpeg 0.8.5
    //frame= 1346 fps=556 q=4.3 Lsize=    2380kB time=00:01:07.29 bitrate= 289.7kbits/s
    //
    // windows 26400
    //frame=  626 fps=224 q=31.0 Lsize=    1879kB time=25.08 bitrate= 613.9kbits/s
    // windows 41074
    //frame=  101 fps=0.0 q=31.0 size=     473kB time=00:00:04.08 bitrate= 950.1kbits/
    //frame=   32 fps=0.0 q=31.0 q=31.0 size=   14770kB time=00:00:01.32 bitrate=91664.9kbits/s
    // Number of stream maps must match number of output streams
    //
    // Error while opening encoder for output stream #0.0 - maybe incorrect parameters such as bit_rate, rate, width or height
    //
    for (int i = 0; i < list.size()-1; i++){
        list[i]=trim(list[i]);

        emit readyData(list[i]);

        // Нормальный ход конвертирования
        if (list[i].mid(0,5) == "frame" or list[i].mid(0,4) == "size"){
            emit changeStatusConvert(parsStatus(list[i]));
        }

        // Ошибки (Не верно подобраны параметры)
        //Кодек не поддерживается "Unsupported codec for output stream #0.0"
        if (list[i].mid(0,27) == "Error while opening encoder"
            or list[i] == "Number of stream maps must match number of output streams"
            or list[i].mid(0,33) == "Could not write header for output"
            or list[i].mid(0,17) == "Unsupported codec"
            or list[i].mid(0,21) == "Video encoding failed"
            or list[i].mid(0,14) == "Invalid value "
            or list[i] == "Output file #0 does not contain any stream"
            or regex("*Invalid data found when processing input*",list[i])==true
            or regex("Encoder * not found for output stream*",list[i])==true
            or regex("Could not write header for output file * (incorrect codec parameters ?)",list[i])==true
            or regex("Stream map '*' matches no streams",list[i])==true
            ){

            emit error(list[i]);
        }
        // Конец конвертирования
        if (list[i].mid(0,5) == "video"){
            emit finish(1);
        }


    }
}

void cConvert::stop(){
    procFFmpeg->close();
}

void cConvert::ffmpeg_finish(int value){
}

sStatusConvert cConvert::parsStatus(QString line){
    sStatusConvert status ;

    //"size=     100kB time=12.77 bitrate=  64.0kbits/s"
    //"frame=  424 fps=422 q=30.3 size=     839kB time=17.73 bitrate= 387.6kbits/s"
    //"frame | 212 fps |   0 q | 31.0 size|     510kB time|8.88 bitrate| 470.3kbits/s"
    //"frame= 1252 fps=132 q=31.0 q=31.0 size=    7191kB time=00:00:50.12 bitrate=1175.3kbits/s"
    //QStringList p=trim(line).split("=");

    int index=-1;
    status.frame="0";
    status.fps="0";
    status.size="0";
    status.time="0";
    status.bitrate="0";

    index=line.indexOf("frame=");
    if (index>=0) status.frame=findValue(line,index+6);

    index=line.indexOf("fps=");
    if (index>=0) status.fps=findValue(line,index+4);

    index=line.indexOf("size=");
    if (index>=0) status.size=findValue(line,index+5);

    index=line.indexOf("time=");
    if (index>=0){
        status.time=findValue(line,index+5);
        if (status.time.split(".").at(0).split(":").size()>1){
            QString str_timer="0";
            QStringList t = status.time.split(".").at(0).split(":");
            double times = t[0].toDouble()*3600+t[1].toDouble()*60+t[2].toDouble();
            if (status.time.split(".").size()==2){
                times+=QString("."+status.time.split(".").at(1)).toDouble();
            }
            str_timer.setNum(times);
            status.time = str_timer;
        }
    }

    index=line.indexOf("bitrate=");
    if (index>=0) status.bitrate=findValue(line,index+8);

    return status;
}

QString cConvert::findValue(QString line,int start){
    int step=0;
    QString value="";
    line+=" ";
    for (int i=start;i<line.length();i++){
        if (step==0){
            if (line.mid(i,1)!=" ") step=i;
        }else{
            if (line.mid(i,1)==" "){
                value=line.mid(step,i-step);
                break;
            }
        }
    }

    return value;
}
