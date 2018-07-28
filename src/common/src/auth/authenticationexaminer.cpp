#include "authenticationexaminer.h"

#include <nextcloudendpointconsts.h>

#include <QMap>

AuthenticationExaminer::AuthenticationExaminer(QObject *parent) :
    QObject(parent)
{

}

bool AuthenticationExaminer::examine(QString serverUrl, AuthenticationMethod method)
{
    const QUrl urlParser = QUrl(serverUrl);
    if (!urlParser.isValid() || urlParser.host().isEmpty()) {
        qDebug() << "Provided server url" << serverUrl << "invalid, aborting examination.";
        Q_EMIT examinationFailed(method);
        return false;
    }

    this->m_tempSettings.setHostString(serverUrl);

    switch (method) {
    case AuthenticationMethod::FLOW_DIALOG:
    {
        HttpGetCommandEntity* flowExamination = new HttpGetCommandEntity(this,
                                                                         NEXTCLOUD_ENDPOINT_LOGIN_FLOW,
                                                                         QMap<QByteArray, QByteArray>(),
                                                                         &this->m_tempSettings);
        QObject::connect(flowExamination, &HttpGetCommandEntity::done,
                         this, [=]() {
            const int statusCode = flowExamination->resultData().toMap()["statusCode"].toInt();
            if (statusCode < 200 || statusCode >= 300)
                Q_EMIT examinationFailed(AuthenticationMethod::FLOW_DIALOG);
            else
                Q_EMIT examinationSucceeded(AuthenticationMethod::FLOW_DIALOG);
            flowExamination->deleteLater();
        });
        QObject::connect(flowExamination, &HttpGetCommandEntity::aborted,
                         this, [=]() {
            Q_EMIT examinationFailed(AuthenticationMethod::FLOW_DIALOG);
            flowExamination->deleteLater();
        });
        flowExamination->run();
        break;
    }
    default:
        break;
    }

    return true;
}
