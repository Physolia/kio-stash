/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -c StashNotifierAdaptor -a stash_adaptor.h:stash_adaptor.cpp ../../dbus/org.kde.kio.StashNotifier.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "stash_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class StashNotifierAdaptor
 */

StashNotifierAdaptor::StashNotifierAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

StashNotifierAdaptor::~StashNotifierAdaptor()
{
    // destructor
}

void StashNotifierAdaptor::addPath(const QString &source, const QString &stashPath, int fileType)
{
    // handle method call org.kde.kio.StashNotifier.addPath
    QMetaObject::invokeMethod(parent(), "addPath", Q_ARG(QString, source), Q_ARG(QString, stashPath), Q_ARG(int, fileType));
}

bool StashNotifierAdaptor::copyWithStash(const QString &src, const QString &dest)
{
    // handle method call org.kde.kio.StashNotifier.copyWithStash
    bool out0;
    QMetaObject::invokeMethod(parent(), "copyWithStash", Q_RETURN_ARG(bool, out0), Q_ARG(QString, src), Q_ARG(QString, dest));
    return out0;
}

QString StashNotifierAdaptor::fileInfo(const QString &path)
{
    // handle method call org.kde.kio.StashNotifier.fileInfo
    QString out0;
    QMetaObject::invokeMethod(parent(), "fileInfo", Q_RETURN_ARG(QString, out0), Q_ARG(QString, path));
    return out0;
}

QStringList StashNotifierAdaptor::fileList(const QString &path)
{
    // handle method call org.kde.kio.StashNotifier.fileList
    QStringList out0;
    QMetaObject::invokeMethod(parent(), "fileList", Q_RETURN_ARG(QStringList, out0), Q_ARG(QString, path));
    return out0;
}

void StashNotifierAdaptor::removePath(const QString &path)
{
    // handle method call org.kde.kio.StashNotifier.removePath
    QMetaObject::invokeMethod(parent(), "removePath", Q_ARG(QString, path));
}

