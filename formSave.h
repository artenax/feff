#ifndef FORMSAVE_H
#define FORMSAVE_H

#include <QDialog>
#include "ui_formSave.h"

class formSave  : public QDialog {
    Q_OBJECT
    public:
        formSave(QWidget *parent = 0);
        QString getProfileName();
        void setProfileName(QString name);

    private:
        Ui::formSave ui;
        QString profileName;

    signals:

    public slots:

    private slots:
        void profile_cancel();
        void profile_save();
        void lineEdit_textChanged(QString text);

};

#endif // FORMSAVE_H
