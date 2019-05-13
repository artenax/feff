#include "cInfoFFmpeg.h"


cInfoFFmpeg::cInfoFFmpeg(QObject *parent) :    QObject(parent){

}



int cInfoFFmpeg::setProgramm(QString _programm){
    //// Установка пути к ffmpeg

    programm=_programm;

    return 1;
}

QString cInfoFFmpeg::getTypeCoder(){
    return typeCoder;
}

QString cInfoFFmpeg::getVersion(){
    /////
    ////     Извлечение версии ffmpeg или avconv
    ///////

    QString data=getData(QStringList() << "-version");
    QStringList list = data.split("\n");

    for (int i = 0; i < list.size(); ++i){
        if  (list[i].mid(0,6).toLower() ==  "ffmpeg" or list[i].mid(0,6).toLower() ==  "avconv"){

            typeCoder=list[i].mid(0,6).toLower();

            // FFmpeg SVN-r22960
            // avconv v0.8-1998-ga7cc78c
            QStringList v = list[i].split(" ");
            if (v.size()==2) return v[1];

            //ffmpeg version N-41074-g9c27f29
            //ffmpeg version 3.2.4 Copyright (c) 2000-2017 the FFmpeg developers
            if (v.size()>2){
                if (v[1]=="version") return v[2].replace(",","");
            }
        }
    }

    typeCoder="";
    return "";
}

QMap<int,sInfoCodec> cInfoFFmpeg::getCodecs(){
    /*
        Codecs:
        D..... = Decoding supported
         .E.... = Encoding supported
        ..V... = Video codec
         ..A... = Audio codec
        ..S... = Subtitle codec
         ...S.. = Supports draw_horiz_band
        ....D. = Supports direct rendering method 1
         .....T = Supports weird frame truncation
        ------
         D V D  4xm             4X Movie
        D V D  8bps            QuickTime 8BPS video
    */

    bool listCodec=false;
    QMap<int,sInfoCodec> codecs;
    sInfoCodec infoCodec;

    QString data=getData(QStringList() << "-codecs");
    QStringList list = data.split("\n");

    int codec_count=0;
    for (int i = 0; i < list.size(); ++i){
        if (list[i] == "") listCodec=false;

        if (listCodec==true){
            QStringList c = list[i].mid(8).split(" ");

            //  Установки по умолчанию
            infoCodec.decoding=false;
            infoCodec.encoding=false;
            infoCodec.typeCodec=0;
            infoCodec.direct_rendering_method_1=false;
            infoCodec.draw_horiz_band=false;
            infoCodec.weird_frame_truncation=false;



            if (list[i].mid(1,1)=="D") infoCodec.decoding=true;
            if (list[i].mid(2,1)=="E") infoCodec.encoding=true;
            if (list[i].mid(3,1)=="V") infoCodec.typeCodec=1;
            if (list[i].mid(3,1)=="A") infoCodec.typeCodec=2;
            if (list[i].mid(3,1)=="S") infoCodec.typeCodec=3;
            if (list[i].mid(4,1)=="S") infoCodec.draw_horiz_band=true;
            if (list[i].mid(5,1)=="D") infoCodec.direct_rendering_method_1=true;
            if (list[i].mid(6,1)=="T") infoCodec.weird_frame_truncation=true;


            infoCodec.name=trim(c[0]);
            infoCodec.desc=trim(list[i].mid(8+c[0].length()));

            codecs.insert(codec_count,infoCodec);
            codec_count++;

        }

        if (list[i].mid(1,3) == "---") listCodec=true;
    }


    return codecs;

}

sInfoSource cInfoFFmpeg::getInfoSource(QString fileName){
    /*
    Input #0, avi, from './aaa.avi':
      Metadata:
        ISFT            : VirtualDubMod 1.5.10.2 (build 2542/release)
      Duration: 00:39:31.49, start: 0.000000, bitrate: 1413 kb/s
        Stream #0.0: Video: mpeg4, yuv420p, 624x352 [PAR 1:1 DAR 39:22], 23.98 fps, 23.98 tbr, 23.98 tbn, 30k tbc
        Stream #0.1: Audio: mp3, 48000 Hz, 2 channels, s16, 256 kb/s
    At least one output file must be specified
    ---
    [matroska @ 0x8112440]Estimating duration from bitrate, this may be inaccurate
    Input #0, matroska, from './Wizards-The.Movie.2009.DVDRip-AVC.[tfile.ru].mkv':
      Duration: 01:33:59.84, start: 0.000000, bitrate: 576 kb/s

        Stream #0.0(eng): Video: h264, yuv420p, 720x400, PAR 1:1 DAR 9:5, 25 fps, 25 tbr, 1k tbn, 50 tbc
        Metadata:
          title           : Заходите к Riper.am (Rip by -=DeMoH=-)

        Stream #0.1(rus): Audio: ac3, 48000 Hz, stereo, s16, 192 kb/s
        Metadata:
          title           : Русский дубляж Нева-фильм

        Stream #0.2(eng): Audio: ac3, 48000 Hz, 5.1, s16, 384 kb/s

    At least one output file must be specified
    */

    sInfoSource infoSource;
    int count_video=0;
    int count_audio=0;
    int count_subtitle=0;
    QString step="";

    // размер файла
    QFile file(fileName);
    infoSource.filesize=IntToStr(file.size()/(1024*1024))+"Mb";
    file.close();

    // имя файла
    QStringList f = fileName.split("/");
    infoSource.fileName=f[f.size()-1];
    infoSource.filePath=fileName.mid(0,fileName.length()-infoSource.fileName.length());

    QString data=getData(QStringList() << "-i" << fileName);
    QStringList list = data.split("\n");

    for (int i = 0; i < list.size(); ++i){
        list[i]=trim(list[i]);
        QStringList s = list[i].split(" ");

        if (trim(s[0]) == "Input"){
            infoSource.format=delLastSymbol(s[2]);
            step="input";
        }
        if (step=="input" and trim(s[0])=="title:"){
            QStringList subtitle=list[i].split(":");
            infoSource.metadata.title=trim(subtitle[1]);
        }

        //Duration: 01:33:59.84, start: 0.000000, bitrate: 576 kb/s
        if (trim(s[0]) == "Duration:"){
            infoSource.duration = delLastSymbol(s[1]);

            if (trim(s[4]) == "bitrate:"){
                infoSource.bitrate = s[5]+" "+s[6];
            }
        }

        //         Stream #0.0(eng): Video: h264, yuv420p, 720x400, PAR 1:1 DAR 9:5, 25 fps, 25 tbr, 1k tbn, 50 tbc
        if (trim(s[0]) == "Stream"){
            if (trim(s[2])=="Video:"){
                count_video++;
                QStringList param = list[i].split(":");
                infoSource.streamVideo[count_video].number=param[0];
                infoSource.streamVideo[count_video].full=list[i].mid(param[0].length()+param[1].length()+2);

                QStringList param2 =list[i].mid(list[i].indexOf("Video:")+6).split(",");
                infoSource.streamVideo[count_video].codec=trim(param2[0]);
                infoSource.streamVideo[count_video].rez1=trim(param2[1]);
                infoSource.streamVideo[count_video].size =trim(param2[2]);

                if (param2[3].mid(param2[3].length()-3) == "fps"){
                    infoSource.streamVideo[count_video].fps =trim(param2[3]);
                }

            }
            step="stream_video";
        }
        if (step=="stream_video" and trim(s[0])=="title:"){
            QStringList title=list[i].split(":");
            infoSource.streamVideo[count_video].metadata.title=trim(title[1]);
        }


        //         Stream #0.1(rus): Audio: ac3, 48000 Hz, stereo, s16, 192 kb/s
        //         Stream #0.1:      Audio: mp2, 44100 Hz, 2 channels, s16, 64 kb/s
        //         Stream #0:1(eng): Audio: vorbis, 48000 Hz, mono, s16 (default)
        if (trim(s[0]) == "Stream"){
            if (trim(s[2])=="Audio:"){
                count_audio++;

                QStringList param = list[i].split(":");
                infoSource.streamAudio[count_audio].number=param[0];
                infoSource.streamAudio[count_audio].full=param[2];

                QStringList param2 =list[i].mid(list[i].indexOf("Audio:")+6).split(",");

                infoSource.streamAudio[count_audio].codec=trim(param2[0]);
                infoSource.streamAudio[count_audio].channels=trim(param2[2]);
                infoSource.streamAudio[count_audio].Sampling_rate=trim(param2[1]);

                if (param2.size()>=5){
                    infoSource.streamAudio[count_audio].bitrate =trim(param2[4]);
                }

            }
            step="stream_audio";
        }
        if (step=="stream_audio" and trim(s[0])=="title:"){
            QStringList title=list[i].split(":");
            infoSource.streamAudio[count_audio].metadata.title=trim(title[1]);
        }


        // Stream #0.9(rus): Subtitle: 0x0000
        if (trim(s[0]) == "Stream"){
            count_subtitle++;
            if (trim(s[2])=="Subtitle:"){
                QStringList param = list[i].split(":");
                infoSource.streamSubtitle[count_subtitle].number=param[0];
                infoSource.streamSubtitle[count_subtitle].full=param[2];
            }
            step="stream_audio";
        }
        if (step=="stream_audio" and trim(s[0])=="title:"){
            QStringList title=list[i].split(":");
            infoSource.streamSubtitle[count_subtitle].metadata.title=trim(title[1]);
        }

    }


    return infoSource;
}

QString cInfoFFmpeg::getData(QStringList arg){
    QString data="";
    QProcess *myProc  = new QProcess();

    // отключить цвет
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("FFMPEG_FORCE_NOCOLOR", "1");
    myProc->setProcessEnvironment(env);


    myProc->start(programm,arg);

    if (myProc->waitForFinished()==false){
       return "";
    }

    data.append(myProc->readAllStandardError());
    data.append(myProc->readAllStandardOutput());

    myProc->deleteLater();

    return data;
}


QString cInfoFFmpeg::getConfiguration(){
    QString data=getData(QStringList() << "");
    QStringList list = data.split("\n");
    for (int i = 0; i < list.size(); ++i){
        QStringList s = trim(list[i]).split(" ");

        if (s[0].toLower()=="configuration:"){
            return list[i];
        }

    }

    return "";
}



QMap<int,sInfoFormat> cInfoFFmpeg::getFormats(){
    bool listFormat=false;
    QMap<int,sInfoFormat> formats;
    sInfoFormat infoFormat;

    QString data=getData(QStringList() << "-formats");
    QStringList list = data.split(QRegExp("\n"));

    int format_count=0;
    for (int i = 0; i < list.size(); ++i){
        if (list[i] == "") listFormat=false;

        if (listFormat==true){
            QStringList c = list[i].mid(4).split(" ");

            //  Установки по умолчанию
            infoFormat.demuxing=false;
            infoFormat.muxing=false;


            if (list[i].mid(1,1)=="D") infoFormat.demuxing=true;
            if (list[i].mid(2,1)=="E") infoFormat.muxing=true;

            infoFormat.name=trim(c[0]);
            infoFormat.desc=trim(list[i].mid(4+c[0].length()));

            formats.insert(format_count,infoFormat);
            format_count++;

        }

        if (list[i].mid(1,2) == "--") listFormat=true;
    }


    return formats;
}

