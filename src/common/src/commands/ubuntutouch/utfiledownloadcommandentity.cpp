#include "utfiledownloadcommandentity.h"
#include <util/filepathutil.h>
#include <util/webdav_utils.h>

#include <QDebug>
#include <QFile>

using namespace Ubuntu::DownloadManager;

UtFileDownloadCommandEntity::UtFileDownloadCommandEntity(QObject* parent,
                                                     QString remotePath,
                                                     QString localPath,
                                                     AccountBase* settings) :
    CommandEntity(parent), m_localPath(localPath), m_settings(settings)
{
    this->m_remotePath = remotePath;
    const QString localDir = localPath.left(localPath.lastIndexOf(QDir::separator())+1);
    this->m_localDir = QDir(localDir);
    const QString fileName = localPath.mid(localDir.length());

    this->m_dlManager = Manager::createSessionManager();
    QObject::connect(this->m_dlManager, &Manager::downloadCreated, this, [=](Download* download){
        this->m_download = download;

        QObject::connect((Download*)this->m_download, SIGNAL(progress(qulonglong, qulonglong)),
                         this, SLOT(handleProgress(qulonglong, qulonglong)));
        QObject::connect((Download*)this->m_download, SIGNAL(finished(const QString&)),
                         this, SLOT(handleFinished(const QString&)));
        QObject::connect((Download*)this->m_download, SIGNAL(error(Error*)),
                         this, SLOT(handleError(Error*)));

        this->m_download->start();
    });

    // extensible list of command properties
    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("davGet");
    info["localPath"] = localPath;
    info["remotePath"] = remotePath;
    info["fileName"] = fileName;
    info["remoteFile"] = remotePath + fileName;
    this->m_commandInfo = CommandEntityInfo(info);
}

UtFileDownloadCommandEntity::~UtFileDownloadCommandEntity()
{
    if (this->m_download) {
        this->m_download->deleteLater();
        this->m_download = Q_NULLPTR;
    }
    if (this->m_dlManager) {
        this->m_dlManager->deleteLater();
        this->m_dlManager = Q_NULLPTR;
    }
}

bool UtFileDownloadCommandEntity::startWork()
{
    if (!CommandEntity::startWork())
        return false;

    if (!this->m_localDir.exists()) {
        const bool mkPathSuccess = this->m_localDir.mkpath(this->m_localDir.absolutePath());
        if (!mkPathSuccess) {
            qWarning() << "Failed to create target path" << this->m_localDir.absolutePath();
            abortWork();
            return false;
        }
    }

    QMap<QByteArray, QByteArray> headers = prepareOcsHeaders(m_settings, QMap<QByteArray, QByteArray>());
    QMap<QString, QString> headerStrings;
    for (const QByteArray& headerKey : headers.keys()) {
        headerStrings.insert(QString::fromUtf8(headerKey),
                             QString::fromUtf8(headers[headerKey]));
    }

    QString remoteUrlPath = FilePathUtil::getWebDavFileUrl(this->m_remotePath, this->m_settings);

    DownloadStruct dlInfo(remoteUrlPath, QVariantMap{}, headerStrings);
    this->m_dlManager->createDownload(dlInfo);

    setState(RUNNING);
    return true;
}

bool UtFileDownloadCommandEntity::abortWork()
{
    const bool success = CommandEntity::abortWork();
    if (this->m_download)
        this->m_download->cancel();
    setState(ABORTED);
    Q_EMIT aborted();
    return success;
}

void UtFileDownloadCommandEntity::handleProgress(qulonglong received, qulonglong total)
{
    setProgress((qreal)((qreal)received/(qreal)total));
}

void UtFileDownloadCommandEntity::handleFinished(const QString& path)
{
    qInfo() << "File download" << this->m_remotePath << "complete.";
    QFile::rename(path, this->m_localPath);
    setState(FINISHED);
    Q_EMIT done();
}

void UtFileDownloadCommandEntity::handleError(Error*)
{
    abortWork();
}
