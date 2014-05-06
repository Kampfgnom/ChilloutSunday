#include "database.h"

#include <model/downloadpackage.h>
#include <model/downloadpart.h>

#include <QPersistence.h>

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>

const char *Database::FileName("database.sqlite");

Database::Database(QObject *parent) :
    QObject(parent)
{
}

bool Database::open(bool cleanDatabase)
{
    QString dataLocationPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QDir dataLocation(dataLocationPath);
    if(!dataLocation.exists()
            && !dataLocation.mkpath(dataLocationPath)) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Could not create data folder!"));
        msg.setInformativeText(tr("mkpath failed for '%1'.")
                               .arg(dataLocationPath));
        msg.exec();
        return false;
    }

    m_file = dataLocation.absoluteFilePath(Database::FileName);

    QFile dbFile(m_file);
    if(!dbFile.exists()) {
        cleanDatabase = true;
        if(!dbFile.open(QFile::WriteOnly)) {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("Could not create database!"));
            msg.setInformativeText(tr("Could not create file '%1'.")
                                   .arg(m_file));
            msg.exec();
            return false;
        }
        dbFile.close();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_file);
    if (!db.open()) {
        QMessageBox msg;
        msg.setIcon(QMessageBox::Critical);
        msg.setText(tr("Could not open database!"));
        msg.setInformativeText(tr("There was an error: '%1'")
                               .arg(db.lastError().text()));
        msg.exec();
        return false;
    }

    Qp::setDatabase(db);
    Qp::registerClass<DownloadPackage>();
    Qp::registerClass<DownloadPart>();

    if(cleanDatabase) {
        QpDatabaseSchema databaseSchema;
        databaseSchema.createCleanSchema();
    }
    else {
        QpDatabaseSchema databaseSchema;
        databaseSchema.adjustSchema();
    }

    return true;
}

QString Database::file() const
{
    return m_file;
}
