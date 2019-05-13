#ifndef FORMINFO_H
#define FORMINFO_H

#include <QDialog>
#include "ui_formInfo.h"
#include "cInfoFFmpeg.h"

class formInfo  : public QDialog {
    Q_OBJECT
    public:
        formInfo(QWidget *parent,sInfoSource infoSource);

    private:
        Ui::formInfo ui;

};

#endif // FORMINFO_H
