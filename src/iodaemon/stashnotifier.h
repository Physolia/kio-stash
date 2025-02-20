/***************************************************************************
 *   Copyright (C) 2016 by Arnav Dhamija <arnav.dhamija@gmail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#ifndef STASHNOTIFIER_H
#define STASHNOTIFIER_H

#include "fs.h"

#include <QStringList>
#include <QVariant>

#include <KDEDModule>

class KDirWatch;

class StashNotifier : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kio.StashNotifier")

private:
    KDirWatch *dirWatch;
    StashFileSystem *fileSystem;
    const QString m_daemonService;
    const QString m_daemonPath;
    QString processString(const QString &path);
    QString encodeString(StashFileSystem::StashNode::iterator node, const QString &path);
    QString encodeString(StashFileSystem::StashNodeData nodeData, const QString &path);

public:
    StashNotifier(QObject* parent, const QList<QVariant>&,
                  const QString &daemonService = "org.kde.kio.StashNotifier",
                  const QString &daemonPath = "/StashNotifier");
    ~StashNotifier();

Q_SIGNALS:
    Q_SCRIPTABLE void listChanged();

public Q_SLOTS:
    Q_SCRIPTABLE void addPath(const QString &source, const QString &stashPath, int fileType);
    Q_SCRIPTABLE void removePath(const QString &path);
    Q_SCRIPTABLE void nukeStash();
    Q_SCRIPTABLE void pingDaemon();
    Q_SCRIPTABLE bool copyWithStash(const QString &src, const QString &dest);
    Q_SCRIPTABLE QStringList fileList(const QString &path);
    Q_SCRIPTABLE QString fileInfo(const QString &path);

private Q_SLOTS:
    void dirty(const QString &path);
    void created(const QString &path);
    void removeWatchedPath(const QString &filePath);
    void displayRoot() //function to the contents of the SFS for testing
    {
        //fileSystem->displayRoot();
    }
};

#endif
