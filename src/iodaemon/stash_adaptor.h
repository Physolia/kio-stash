/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c StashNotifierAdaptor -a stash_adaptor.h:stash_adaptor.cpp ../../dbus/org.kde.kio.StashNotifier.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef STASH_ADAPTOR_H
#define STASH_ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface org.kde.kio.StashNotifier
 */
class StashNotifierAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kio.StashNotifier")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.kde.kio.StashNotifier\">\n"
"    <signal name=\"listChanged\"/>\n"
"    <method name=\"addPath\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"source\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"stashPath\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"fileType\"/>\n"
"    </method>\n"
"    <method name=\"removePath\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"path\"/>\n"
"    </method>\n"
"    <method name=\"fileList\">\n"
"      <arg direction=\"out\" type=\"as\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"path\"/>\n"
"    </method>\n"
"    <method name=\"fileInfo\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"path\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    StashNotifierAdaptor(QObject *parent);
    virtual ~StashNotifierAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void addPath(const QString &source, const QString &stashPath, int fileType);
    QString fileInfo(const QString &path);
    QStringList fileList(const QString &path);
    void removePath(const QString &path);
Q_SIGNALS: // SIGNALS
    void listChanged();
};

#endif
