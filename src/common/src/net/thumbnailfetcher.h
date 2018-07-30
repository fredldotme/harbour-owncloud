#ifndef THUMBNAILFETCHER_H
#define THUMBNAILFETCHER_H

#include <QObject>
#include "abstractfetcher.h"
#include <commandqueue.h>
#include <settings/nextcloudsettingsbase.h>
#include <qwebdav.h>

class ThumbnailFetcher : public AbstractFetcher
{
    Q_OBJECT

    Q_PROPERTY(QString remoteFile READ remoteFile WRITE setRemoteFile NOTIFY remoteFileChanged)

public:
    explicit ThumbnailFetcher(QObject *parent = Q_NULLPTR);

    void setRemoteFile(QString v);
    QString remoteFile();

public slots:
    void fetch() Q_DECL_OVERRIDE;

private:
    QString m_remoteFile;

signals:
    void remoteFileChanged();

};
Q_DECLARE_METATYPE(ThumbnailFetcher*)

#endif // THUMBNAILFETCHER_H
