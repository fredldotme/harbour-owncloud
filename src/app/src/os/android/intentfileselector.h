#ifndef INTENTFILESELECTOR_H
#define INTENTFILESELECTOR_H

#include <QObject>
#include "intentfileselectorreceiver.h"

class IntentFileSelector : public QObject
{
    Q_OBJECT
public:
    explicit IntentFileSelector(QObject *parent = nullptr);

public slots:
    void fileSelectIntent();

private:
    IntentFileSelectorReceiver m_resultReceiver;

signals:
    void fileSelected(const QString& filePath);

};

#endif // INTENTFILESELECTOR_H
