# #####################################################################
# Automatically generated by qmake (2.01a) ?? ????. 26 18:57:14 2010
# #####################################################################
TEMPLATE = app
TARGET = feff
DEPENDPATH += .
INCLUDEPATH += .

DESTDIR = Bin

MOC_DIR = .build/.moc
RCC_DIR = .build/.rcc
UI_DIR = .build/.ui
OBJECTS_DIR = .build/.obj
QMAKE_LINK_OBJECT_SCRIPT = .build/.obj

QT += widgets

win32:RC_FILE = feff.rc

RESOURCES += feff.qrc

TRANSLATIONS = $$files(langs/feff_*.ts)

### install ###

isEmpty(QMAKE_LRELEASE) {
    win32|os2:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    unix {
        !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease-qt5 }
    } else {
        !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease }
    }
}

!win32 {
  system($${QMAKE_LRELEASE} -silent $${_PRO_FILE_} 2> /dev/null)
}
win32 {
  system($${QMAKE_LRELEASE} $${_PRO_FILE_})
}

updateqm.input = TRANSLATIONS
updateqm.output = langs/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE -silent ${QMAKE_FILE_IN} -qm langs/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

data_bin.path = /usr/bin/
data_bin.files = Bin/feff
INSTALLS += data_bin

data_app.path = /usr/share/applications/
data_app.files = pkg/feff.desktop
INSTALLS += data_app

data_pixmaps.path = /usr/share/pixmaps/
data_pixmaps.files = feff.png
INSTALLS += data_pixmaps

data_langs.path = /usr/share/feff/langs/
data_langs.files = langs/*.qm
INSTALLS += data_langs

# Input
SOURCES += main.cpp \
    form1.cpp \
    cInfoFFmpeg.cpp \
    public.cpp \
    cConvert.cpp \
    formSave.cpp \
    formInfo.cpp

HEADERS += form1.h \
    cInfoFFmpeg.h \
    public.h \
    cConvert.h \
    formSave.h \
    formInfo.h

FORMS += mainForm.ui \
    formSave.ui \
    formInfo.ui
