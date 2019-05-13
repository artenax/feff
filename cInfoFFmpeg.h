#ifndef CINFOFFMPEG_H
#define CINFOFFMPEG_H

#include <QObject>
#include <QtGui>
#include "public.h"


/*
D..... = Decoding supported
.E.... = Encoding supported
..V... = Video codec (1)
..A... = Audio codec (2)
..S... = Subtitle codec (3)
...S.. = Supports draw_horiz_band
....D. = Supports direct rendering method 1
.....T = Supports weird frame truncation
*/
struct sInfoCodec {
    QString name;
    QString desc;
    bool decoding;
    bool encoding;
    int typeCodec;
    bool draw_horiz_band;
    bool direct_rendering_method_1;
    bool weird_frame_truncation;
};


/*
File formats:
D. = Demuxing supported
.E = Muxing supported
--
E 3g2             3GP2 format
*/
struct sInfoFormat {
    QString name;
    QString desc;
    bool demuxing;
    bool muxing;
};

struct sMetadata {
    QString title;
    QString ISFT;

};

/*Stream #0.0(eng): Video: h264, yuv420p, 720x400, PAR 1:1 DAR 9:5, 25 fps, 25 tbr, 1k tbn, 50 tbc
Metadata:
  title           : Заходите к Riper.am (Rip by -=DeMoH=-)*/
struct sStreamVideo {
    QString full;
    QString number;
    QString codec;
    QString rez1;
    QString size;
    QString rez2;
    QString fps;
    QString tbn;
    QString tbc;
    sMetadata metadata;
};

//Stream #0.1(rus): Audio: ac3, 48000 Hz, 5.1, s16, 384 kb/s
struct sStreamAudio {
    QString full;
    QString number;
    QString codec;
    QString Sampling_rate;
    QString channels;
    QString rez1;
    QString bitrate;
    sMetadata metadata;
};

//Stream #0.2(rus): Subtitle: 0x0000
struct sStreamSubtitle {
    QString full;
    QString number;
    QString codec;
    sMetadata metadata;
};


struct sInfoSource {
    QString fileName;
    QString filePath;
    QString filesize;
    QString format;
    QString duration;
    QString bitrate;
    sMetadata metadata;
    sStreamVideo streamVideo[16];
    sStreamAudio streamAudio[16];
    sStreamSubtitle streamSubtitle[16];
};


class cInfoFFmpeg : public QObject {
    Q_OBJECT
    public:
        explicit cInfoFFmpeg(QObject *parent = 0);
        int setProgramm(QString programm);
        QString getVersion();
        QString getConfiguration();
        QMap<int,sInfoCodec>  getCodecs();
        QMap<int,sInfoFormat> getFormats();
        sInfoSource getInfoSource(QString fileName);
        QString getTypeCoder();

    private:
        QString getData(QStringList arg);
        QString programm;

        QString typeCoder;

    signals:

    public slots:

};

#endif // CINFOFFMPEG_H
