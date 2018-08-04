#ifndef OCSNETACCESSFACTORY_H
#define OCSNETACCESSFACTORY_H

#include <QObject>
#include <QQmlNetworkAccessManagerFactory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QQmlAbstractUrlInterceptor>

#include <settings/nextcloudsettingsbase.h>

class OscNetAccess : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit OscNetAccess(QObject* parent = Q_NULLPTR,
                          NextcloudSettingsBase* settings = Q_NULLPTR);

private:
    NextcloudSettingsBase* m_settings = Q_NULLPTR;

public:
    QNetworkReply* createRequest(Operation op,
                                 const QNetworkRequest &request,
                                 QIODevice* outgoingData) Q_DECL_OVERRIDE;
};

class OcsNetAccessFactory :
        //public QQmlAbstractUrlInterceptor,
        public QQmlNetworkAccessManagerFactory
{
public:
    OcsNetAccessFactory(NextcloudSettingsBase* settings = Q_NULLPTR);
    OcsNetAccessFactory(const OcsNetAccessFactory& o);

    //QUrl intercept(const QUrl& url, DataType type) Q_DECL_OVERRIDE;
    QNetworkAccessManager* create(QObject *parent) Q_DECL_OVERRIDE;

private:
    NextcloudSettingsBase* m_settings = Q_NULLPTR;
};


#endif // OCSNETACCESSFACTORY_H
