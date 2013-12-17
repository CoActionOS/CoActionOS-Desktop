#ifndef BUILDGENERATOR_H
#define BUILDGENERATOR_H

#include <QObject>
#include <QString>

class CBuildGenerator : public QObject
{
    Q_OBJECT
public:

    typedef struct {
        QString cflags;
        QString cxxflags;
        QString ldflags;
        QString ldadd;
    } BuildFlags;

    explicit CBuildGenerator(QObject *parent = 0);


    static int generateMain(QString projectPath, QString * errorMessage, bool cpp);
    static int generateConfigureAc(QString projectPath,
                                   QString projectName,
                                   QString version,
                                   QString contactEmail, QString * errorMessage);
    static int generateMakefileAm(QString projectPath,
                                  QString projectName,
                                  BuildFlags * flags,
                                  QString sources,
                                  QString * errorMessage);

    static int generateMakefile(QString projectPath,
                                QString projectName,
                                BuildFlags * flags,
                                QString pgmStart,
                                QString dataStart,
                                QString dataSize,
                                QString prefix,
                                QString arch,
                                QString * errorMessage);

signals:

public slots:

};

#endif // BUILDGENERATOR_H
