#ifndef DUMMYINTENTFILESELECTOR_H
#define DUMMYINTENTFILESELECTOR_H

#include <QObject>

class DummyIntentFileSelector : public QObject
{
    Q_OBJECT
public:
    explicit DummyIntentFileSelector(QObject *parent = nullptr);

public slots:
    void fileSelectIntent();

signals:
    void fileSelected(const QString& filePath);

};

#endif // DUMMYINTENTFILESELECTOR_H
