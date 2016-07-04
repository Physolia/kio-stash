/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c StashNotifierAdaptor -a stash_adaptor.h:stash_adaptor.cpp dbus/org.kde.kio.StashNotifier.xml
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

void StashNotifierAdaptor::addPath(const QString &path, const QString &currentDir)
{
    // handle method call org.kde.kio.StashNotifier.addPath
    QMetaObject::invokeMethod(parent(), "addPath", Q_ARG(QString, path), Q_ARG(QString, currentDir));
}

void StashNotifierAdaptor::fileList()
{
    // handle method call org.kde.kio.StashNotifier.fileList
    QMetaObject::invokeMethod(parent(), "fileList");
}

void StashNotifierAdaptor::removePath(const QString &path)
{
    // handle method call org.kde.kio.StashNotifier.removePath
    QMetaObject::invokeMethod(parent(), "removePath", Q_ARG(QString, path));
}

