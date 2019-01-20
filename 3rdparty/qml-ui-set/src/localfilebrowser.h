#ifndef LOCALFILEBROWSER_H
#define LOCALFILEBROWSER_H

#include <QObject>
#include <QDir>
#include <QStandardPaths>
#include <QVariantList>

class LocalFileBrowser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString pathRestriction READ path WRITE setPathRestriction NOTIFY pathRestrictionChanged)
    Q_PROPERTY(QVariantList content READ content WRITE setContent NOTIFY contentChanged)

public:
    LocalFileBrowser(QObject *parent = 0);

public slots:
    void cd(QString newPath,
                    QString pathRestriction);
    void cd(QString newPath);
    void cd();
    QVariantList mountedVolumes();

private:
    QString path();
    void setPath(QString value);
    QString pathRestriction();
    void setPathRestriction(QString value);
    QVariantList content();
    void setContent(QVariantList content);

    QString m_path;
    QString m_pathRestriction;
    QVariantList m_content;

signals:
    void pathRestrictionChanged();
    void pathChanged();
    void contentChanged();
    void errorOccured(QString error);
};
Q_DECLARE_METATYPE(LocalFileBrowser*)

#endif // LOCALFILEBROWSER_H
