#include "ui/mainwindow.h"
#include <QApplication>

#include <QOpenGLContext>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Configuration of the default OpenGL surface format
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setMinorVersion(3);
    format.setMajorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setAlphaBufferSize(0);
    format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);
    format.setOption(QSurfaceFormat::DebugContext);
    QSurfaceFormat::setDefaultFormat(format);

    // Enable OpenGL context sharing by default
    a.setAttribute(Qt::AA_ShareOpenGLContexts);

    MainWindow w;
    w.show();

    return a.exec();
}
