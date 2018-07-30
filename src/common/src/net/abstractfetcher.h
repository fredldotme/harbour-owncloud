#ifndef ABSTRACTFETCHER_H
#define ABSTRACTFETCHER_H

#include <QObject>
#include <commandqueue.h>
#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>

class AbstractFetcher : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(NextcloudSettingsBase* settings READ settings WRITE setSettings NOTIFY settingsChanged)
    Q_PROPERTY(CommandQueue* commandQueue READ commandQueue WRITE setCommandQueue NOTIFY commandQueueChanged)
    Q_PROPERTY(qint16 width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qint16 height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(bool fetching READ fetching NOTIFY fetchingChanged)

public:
    explicit AbstractFetcher(QObject *parent = Q_NULLPTR);

public slots:
    virtual void fetch() = 0;

protected:
    NextcloudSettingsBase* settings();
    void setSettings(NextcloudSettingsBase* v);
    CommandQueue* commandQueue();
    void setCommandQueue(CommandQueue* v);
    QString source();
    void setSource(const QString& v);
    qint16 width();
    void setWidth(qint16 v);
    qint16 height();
    void setHeight(qint16 v);
    bool fetching();
    void setFetching(bool v);

private:
    QString m_source;
    qint16 m_width = 0;
    qint16 m_height = 0;
    bool m_fetching = false;
    NextcloudSettingsBase* m_settings = Q_NULLPTR;
    CommandQueue* m_commandQueue = Q_NULLPTR;
    QWebdav* m_client = Q_NULLPTR;

signals:
    void sourceChanged();
    void settingsChanged();
    void commandQueueChanged();
    void widthChanged();
    void heightChanged();
    void fetchingChanged();
};
Q_DECLARE_METATYPE(AbstractFetcher*)

#endif // ABSTRACTFETCHER_H
