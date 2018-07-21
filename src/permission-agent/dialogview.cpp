#include "dialogview.h"

#include <QGuiApplication>
#include <QOpenGLContext>
#include <qpa/qplatformnativeinterface.h>

DialogView::DialogView(const QUrl &source, QWindow *parent) :
    QQuickView(source, parent)
{
    m_nativeHandle = QGuiApplication::platformNativeInterface();
    connect(this, &DialogView::openglContextCreated, this,
            [=](QOpenGLContext *context) {
        m_nativeHandle->setWindowProperty(handle(), QLatin1String("CATEGORY"), "dialog");
    });
}
