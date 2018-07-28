#ifndef OCSNETACCESSFACTORY_H
#define OCSNETACCESSFACTORY_H

#include <QObject>
#include <QQmlNetworkAccessManagerFactory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class OscNetAccess : public QNetworkAccessManager
{
public:
    explicit OscNetAccess(QObject* parent = Q_NULLPTR);

protected:
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice* outgoingData);
};

class OcsNetAccessFactory : public QQmlNetworkAccessManagerFactory
{
public:
    QNetworkAccessManager* create(QObject *parent) Q_DECL_OVERRIDE;
};


#endif // OCSNETACCESSFACTORY_H
