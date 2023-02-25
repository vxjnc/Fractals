QT += core widgets gui opengl

QMAKE_CXXFLAGS += -std=c++17 -fopenmp -pipe -O2 -Wall
QMAKE_LFLAGS += -lOpenCL -fopenmp
QMAKE_LIBS += -fopenmp -lgomp -lpthread -lOpenCL

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050000

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Main.cpp \
    MainWindow/MainWindow.cpp \
    vec2.cpp \
    vec3.cpp \
    vec4.cpp \
    AlgebraicFractal/AlgebraicFractal.cpp \
    GeometricFractal/GeometricFractal.cpp \
    StochasticFractal/StochasticFractal.cpp \
    PaletteWindow/PaletteWindow.cpp \
    DoubleRoundSpinBox/DoubleRoundSpinBox.cpp

HEADERS += \
    MainWindow/MainWindow.h \
    AlgebraicFractal/AlgebraicFractal.h \
    GeometricFractal/GeometricFractal.h \
    StochasticFractal/StochasticFractal.h \
    PaletteWindow/PaletteWindow.h \
    DoubleRoundSpinBox/DoubleRoundSpinBox.h

FORMS += \
    MainWindow/MainWindow.ui \
    PaletteWindow/PaletteWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

#LIBS += -L"C:/Program Files (x86)/AMD APP SDK/2.9/lib/x86/" -lOpenCL

#INCLUDEPATH += "C:/Program Files (x86)/AMD APP SDK/2.9/include/"
#                "C:/Program Files (x86)/AMD APP SDK/2.9/lib/x86/"
#DEPENDPATH += "C:/Program Files (x86)/AMD APP SDK/2.9/include/"

#PRE_TARGETDEPS += "C:/Program Files (x86)/AMD APP SDK/2.9/lib/x86/OpenCL.lib"
