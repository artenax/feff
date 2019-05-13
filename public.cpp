#include "public.h"
#include <QStringList>


QString trim(QString str){
    str.replace(QRegExp("^\\s+"),"");
    str.replace(QRegExp("\\s+$"),"");
    return str;
}

QString IntToStr(int buf){
    QString tmp;
    tmp.setNum(buf,10);

    return tmp;
}

QString IntToStr(uint buf){
    QString tmp;
    tmp.setNum(buf,10);

    return tmp;
}

QString IntToStr(long buf){
    QString tmp;
    tmp.setNum(buf,10);

    return tmp;
}
QString IntToStr(qint64 buf){
    QString tmp;
    tmp.setNum(buf,10);

    return tmp;
}


QString DoubleToStr(double buf){
    QString tmp;
    tmp.setNum(buf,'f',2);

    return tmp;
}


int StrToInt(QString buf){
    return buf.toInt();
}

double StrToDouble(QString buf){
    return buf.toDouble();
}


QString iff(bool ext,QString yes,QString no){
    if (ext) return yes;

    return no;
}


QString delLastSymbol(QString str){
    // удаление последнего символа + убрать пробелы по бокам
    str.replace(QRegExp("^\\s+"),"");
    str.replace(QRegExp("\\s+$"),"");
    return str.mid(0,str.length()-1);
}

bool regex(QString pattern,QString match){
    QRegExp test(pattern);
    test.setPatternSyntax(QRegExp::RegExp);
    return test.exactMatch(match);
}
