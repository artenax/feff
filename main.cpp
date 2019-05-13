#include <QApplication>

#include "form1.h"

int main(int argc,char** argv) {
    QApplication app(argc,argv);


    QDir::setCurrent(QApplication::applicationDirPath());


    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

    QTranslator translator;
    QString locale = QLocale::system().name();


    if (translator.load(QString("feff_") + locale)==false){
        translator.load(QString("/usr/share/feff/feff_") + locale);
    }
    app.installTranslator(&translator);


    form1 *form = new form1(argc,argv);
    form->show();

    
    return app.exec();
}

