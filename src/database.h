#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);

    bool open(bool cleanDatabase);

    QString fileName() const;

    static const char *FileName;
private:
    QString m_fileName;
};

#endif // DATABASE_H
