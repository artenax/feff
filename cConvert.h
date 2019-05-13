#ifndef CCONVERT_H
#define CCONVERT_H

#include <QWidget>
#include <QtGui>
#include "public.h"

struct sStatusConvert {
    QString frame;
    QString fps;
    QString q;
    QString size;
    QString time;
    QString bitrate;
};

class cConvert : public QWidget {
    Q_OBJECT
    public:
        explicit cConvert(QWidget *parent = 0);
        void start(QString pathFFmpeg,QStringList arg);
        void stop();

    private:
        sStatusConvert parsStatus(QString line);
        QString findValue(QString line,int start);

        QString pathFFmpeg;
        QString dataOutput;
        QProcess *procFFmpeg;

    signals:
        void changeStatusConvert(sStatusConvert);
        void readyData(QString);
        void finish(int value);
        void error(QString);

    public slots:

    private slots:
        void ffmpeg_readyRead();
        void ffmpeg_finish(int value);


};

#endif // CCONVERT_H
