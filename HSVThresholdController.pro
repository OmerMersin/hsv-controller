QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# OpenCV configuration
INCLUDEPATH += "C:/Users/Getac/Downloads/opencv/build/include"

# Link against opencv_world library for both debug and release
win32:CONFIG(release, debug|release): LIBS += -L"C:/Users/Getac/Downloads/opencv/build/x64/vc16/lib" -lopencv_world490
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Users/Getac/Downloads/opencv/build/x64/vc16/lib" -lopencv_world490d

RESOURCES += \
    icon.qrc

RC_FILE  += \
    icon.rc
