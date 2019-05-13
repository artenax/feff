#include "formSave.h"

formSave::formSave(QWidget *parent) : QDialog(parent){
    ui.setupUi(this);
    this->setFixedSize(this->width(),this->height());


    this->setWindowTitle(tr("Save profile"));
    this->setWindowIcon(QIcon(":feff.ico"));

    connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(profile_cancel()));
    connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(profile_save()));
    connect(ui.lineEdit,SIGNAL(textChanged(QString)),this,SLOT(lineEdit_textChanged(QString)));


}



void formSave::profile_cancel(){
    profileName="";
    hide();
}

void formSave::profile_save(){
    profileName=ui.lineEdit->text();
    hide();
}

void formSave::setProfileName(QString name){
    ui.lineEdit->setText(name);
    if (name.length()==0){
        ui.pushButton_2->setEnabled(0);
    }
}

QString formSave::getProfileName(){
    return profileName;
}

void formSave::lineEdit_textChanged(QString text){
    ui.pushButton_2->setEnabled(1);

    if (ui.lineEdit->text().length()==0){
        ui.pushButton_2->setEnabled(0);
    }
}
