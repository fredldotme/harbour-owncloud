#ifndef DIALOGVIEW_H
#define DIALOGVIEW_H

#include <QQuickView>

class QPlatformNativeInterface;
class DialogView : public QQuickView
{
    Q_OBJECT
public:
    explicit DialogView(const QUrl &source = QUrl(),
                        QWindow *parent = Q_NULLPTR);

private:
    QPlatformNativeInterface* m_nativeHandle = nullptr;

signals:

public slots:
};

#endif // DIALOGVIEW_H
