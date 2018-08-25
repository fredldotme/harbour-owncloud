#ifndef FLOWLOGINAUTHENTICATOR_H
#define FLOWLOGINAUTHENTICATOR_H

#include <QObject>
#include "authenticator.h"
#include <nextcloudendpointconsts.h>

class FlowLoginAuthenticator : public Authenticator
{
    Q_OBJECT

    Q_PROPERTY(QString url READ url NOTIFY urlChanged)
public:
    explicit FlowLoginAuthenticator(QObject *parent = Q_NULLPTR,
                                    NextcloudSettingsBase* commandQueue = Q_NULLPTR);

    QString url() {
        if (!this->settings())
            return QStringLiteral("");
        return this->settings()->hoststring() + NEXTCLOUD_ENDPOINT_LOGIN_FLOW;
    }

protected:
    void updateClientSettings() Q_DECL_OVERRIDE {}

public slots:
    void authenticate() Q_DECL_OVERRIDE;
    void validateFlowResponse(QString responseUrl);

signals:
    void urlChanged();
};

#endif // FLOWLOGINAUTHENTICATOR_H
