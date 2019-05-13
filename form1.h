#ifndef FORM1_H
#define FORM1_H

#include <QWidget>
#include <QtGui>
#include "cInfoFFmpeg.h"
#include "cConvert.h"
#include "formSave.h"
#include "formInfo.h"
#include "ui_mainForm.h"


class form1 : public QWidget {
    Q_OBJECT
    public:
        explicit form1(int argc,char** argv);

    private:
        Ui::mainForm ui;
        int setFFmpeg(QString fileName);
        int setSource(QString fileName);
        void formWait(QString message,bool visible);
        double convTime(QString duration);
        void Convert();
        void initFFmpegCodecs();
        void initFFmpegFormats();
        void initFFmpegResolution();
        void initFFmpegAspect();
        void initFFmpegBitrate();
        void initFFmpegSampling();
        void initFFmpegVideoFrame();
        void initFFmpegChannels();
        void initProfiles();
        void initAbout();
        void parsCommandLine(int argc,char** argv);


        cConvert *convert;

        sInfoSource infoSource;

        int btnConvert;
        int debug;

        QSettings *confProfiles;

        cInfoFFmpeg infoFFmpeg;
        QMap<int,sInfoCodec> ffmpeg_codecs;
        QMap<int,sInfoFormat> ffmpeg_formats;

        bool autoEditExtension;

        QString pathFFmpeg;

        QWidget *tabMessage;

        QProcess *ffmpeg ;


    signals:

    public slots:
        void openFFmpeg();
        void openSource();
        void openTarget();
        void Click_pushButton_5();
        void convert_changeStatusConvert(sStatusConvert);
        void convert_readyData(QString text);
        void ffmpeg_finish(int value);
        void ffmpeg_error(QString err_str);
        void profile_changed(QString);
        void profile_delete();
        void profile_save();
        void showFormInfo();
        void formatEdit(int index);

    protected:
        bool eventFilter(QObject*, QEvent *);

};

#endif // FORM1_H
