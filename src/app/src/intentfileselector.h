#ifndef INTENTFILESELECTOR_H
#define INTENTFILESELECTOR_H

#include <QObject>

class IntentFileSelector : public QObject
{
    Q_OBJECT
public:
    explicit IntentFileSelector(QObject *parent = nullptr);

public slots:
    void fileSelectIntent();

signals:

};

#endif // INTENTFILESELECTOR_H
