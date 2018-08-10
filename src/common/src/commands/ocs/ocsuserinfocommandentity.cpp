#include "ocsuserinfocommandentity.h"

#include <QVariant>
#include <nextcloudendpointconsts.h>

#include <QDebug>
#include <QXmlSimpleReader>

class UserInfoXmlHandler : public QXmlDefaultHandler {
public:
    bool startElement(const QString&, const QString&,
                      const QString &name, const QXmlAttributes&)
    {
        if (name == QStringLiteral("enabled"))
            this->m_inTag = Enabled;
        else if (name == QStringLiteral("displayname"))
            this->m_inTag = DisplayName;
        else if (name == QStringLiteral("email"))
            this->m_inTag = EMail;
        else if (name == QStringLiteral("free"))
            this->m_inTag = Free;
        else if (name == QStringLiteral("used"))
            this->m_inTag = Used;
        else if (name == QStringLiteral("total"))
            this->m_inTag = Total;
        else
            this->m_inTag = None;

        return true;
    }
    bool characters(const QString & ch)
    {
        const QString value = ch.trimmed();
        if (value.isEmpty())
            return true;

        qDebug() << m_inTag << value;
        if (this->m_inTag == Enabled)
            this->m_enabled = (value == QStringLiteral("true"));
        else if (this->m_inTag == DisplayName)
            this->m_displayName = value;
        else if (this->m_inTag == EMail)
            this->m_email = value;
        else if (this->m_inTag == Free)
            this->m_free = value;
        else if (this->m_inTag == Used)
            this->m_used = value;
        else if (this->m_inTag == Total)
            this->m_total = value;
        return true;
    }

    bool enabled() { return this->m_enabled; }
    QString displayName() { return this->m_displayName; }
    QString email() { return this->m_email; }
    QString free() { return this->m_free; }
    QString used() { return this->m_used; }
    QString total() { return this->m_total; }

private:
    enum UserInfoTag { None = 0, Enabled, DisplayName,
                       EMail, Free, Used, Total };

    UserInfoTag m_inTag = None;
    bool m_enabled = false;
    QString m_displayName;
    QString m_email;
    QString m_free;
    QString m_used;
    QString m_total;
};

QString prepareUserinfoPath(NextcloudSettingsBase* settingsBase)
{
    const QString ret = NEXTCLOUD_ENDPOINT_OCS_USERINFO +
            (settingsBase ? settingsBase->username() : QStringLiteral(""));
    return ret;
}

OcsUserInfoCommandEntity::OcsUserInfoCommandEntity(QObject *parent,
                                                   NextcloudSettingsBase* settings,
                                                   QMap<QByteArray, QByteArray> headers) :
    OcsCommandEntity(parent, prepareUserinfoPath(settings), headers, settings)
{
    QMap<QString, QVariant> info;
    info["type"] = QStringLiteral("userInfo");
    this->m_commandInfo = CommandEntityInfo(info);

    QObject::connect(this, &OcsUserInfoCommandEntity::contentReady,
                     this, [=](){
        QVariantMap resultMap = this->m_resultData;
        const int statusCode = resultMap["statusCode"].toInt();
        if (statusCode < 200 || statusCode >= 300)
            return;

        const QByteArray content = resultMap["content"].toByteArray();
        if (content.isEmpty())
            return;

        qDebug() << content;

        QXmlSimpleReader xmlReader;
        QXmlInputSource xmlInputSource;
        UserInfoXmlHandler handler;
        xmlInputSource.setData(content);
        xmlReader.setContentHandler(&handler);
        if (!xmlReader.parse(&xmlInputSource))
            return;

        resultMap.insert("enabled", handler.enabled());
        resultMap.insert("displayName", handler.displayName());
        resultMap.insert("email", handler.email());
        resultMap.insert("freeBytes", handler.free());
        resultMap.insert("usedBytes", handler.used());
        resultMap.insert("totalBytes", handler.total());
        this->m_resultData = resultMap;
    });
}

bool OcsUserInfoCommandEntity::startWork()
{
    const bool canContinue = OcsCommandEntity::startWork();
    if (!canContinue) {
        qWarning() << "Cannot request user information";
        abortWork();
        return false;
    }

    return true;
}
