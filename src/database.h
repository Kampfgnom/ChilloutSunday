#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);

    bool open(bool cleanDatabase);

    static const char *FileName;

    QString file() const;

private:
    QString m_file;
};

#endif // DATABASE_H
