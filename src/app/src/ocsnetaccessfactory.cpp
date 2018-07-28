#include "ocsnetaccessfactory.h"

#include <QDebug>

OscNetAccess::OscNetAccess(QObject *parent) : QNetworkAccessManager(parent)
{

}

QNetworkRequest getOcsRequest(const QNetworkRequest& request)
{
    QNetworkRequest ret(request);
    ret.setRawHeader(QByteArrayLiteral("OCS-APIREQUEST"),
                     QByteArrayLiteral("true"));
    qDebug() << "getOcsRequest()";
    return ret;
}

QNetworkReply* OscNetAccess::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    return QNetworkAccessManager::createRequest(op, getOcsRequest(request), outgoingData);
}

QNetworkAccessManager* OcsNetAccessFactory::create(QObject *parent)
{
    qDebug() << Q_FUNC_INFO;
    OscNetAccess *nam = new OscNetAccess(parent);
    return nam;
}
