#ifndef PUBLIC_H
#define PUBLIC_H

#include <QString>

extern QString trim(QString);
extern QString iff(bool ext,QString yes,QString no);


extern int StrToInt(QString);
extern double StrToDouble(QString);
extern QString IntToStr(int);
extern QString IntToStr(uint);
extern QString IntToStr(quint32);
extern QString IntToStr(qint64);
extern QString IntToStr(long);
extern QString DoubleToStr(double);
extern QString delLastSymbol(QString str);
extern bool regex(QString pattern,QString match);

extern QString ntoa(quint32 ipaddr);


#endif // PUBLIC_H

