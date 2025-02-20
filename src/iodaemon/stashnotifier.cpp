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

#include "stashnotifier.h"
#include "stash_adaptor.h"

#include <KDirWatch>
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(StashNotifierFactory, "stashnotifier.json", registerPlugin<StashNotifier>();)

StashNotifier::StashNotifier(QObject *parent, const QList<QVariant> &var,
                             const QString &daemonService,
                             const QString &daemonPath) :
    KDEDModule(parent),
    m_daemonService(daemonService),
    m_daemonPath(daemonPath)
{
    dirWatch = new KDirWatch(this);
    qDebug() << "Launching stash daemon.";

    new StashNotifierAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject(m_daemonPath, this);
    dbus.registerService(m_daemonService);

    fileSystem = new StashFileSystem(parent);

    connect(dirWatch, &KDirWatch::dirty, this, &StashNotifier::dirty);
    connect(dirWatch, &KDirWatch::created, this, &StashNotifier::created);
    connect(dirWatch, &KDirWatch::deleted, this, &StashNotifier::removeWatchedPath);
    connect(this, &StashNotifier::listChanged, this, &StashNotifier::displayRoot);
}

StashNotifier::~StashNotifier()
{
}

QString StashNotifier::encodeString(StashFileSystem::StashNode::iterator node, const QString &path) //format type::stashpath::source
{
    QString encodedString;

    switch (node.value().type) {
    case StashFileSystem::NodeType::DirectoryNode:
        encodedString = "dir";
        break;
    case StashFileSystem::NodeType::FileNode:
        encodedString = "file";
        break;
    case StashFileSystem::NodeType::SymlinkNode:
        encodedString = "symlink";
        break;
    case StashFileSystem::NodeType::InvalidNode:
        encodedString = "invalid";
        break;
    }

    if (path == "/") {
        encodedString += "::" + QStringLiteral("/") + node.key();
    } else {
        encodedString += "::" + path + QStringLiteral("/") + node.key();
    }

    if (node.value().type == StashFileSystem::NodeType::FileNode ||
            node.value().type == StashFileSystem::NodeType::SymlinkNode) {
        encodedString += "::" + node.value().source;
    } else {
        encodedString += "::";
    }

    return encodedString;
}

QString StashNotifier::encodeString(StashFileSystem::StashNodeData nodeData, const QString &path)
{
    QString encodedString;

    switch (nodeData.type) {
    case StashFileSystem::NodeType::DirectoryNode:
        encodedString = "dir";
        break;
    case StashFileSystem::NodeType::FileNode:
        encodedString = "file";
        break;
    case StashFileSystem::NodeType::SymlinkNode:
        encodedString = "symlink";
        break;
    case StashFileSystem::NodeType::InvalidNode:
        encodedString = "invalid";
        break;
    }

    encodedString += "::" + path;

    if (nodeData.type == StashFileSystem::NodeType::FileNode ||
            nodeData.type == StashFileSystem::NodeType::SymlinkNode) {
        encodedString += "::" + nodeData.source;
    } else {
        encodedString += "::";
    }

    return encodedString;
}

QStringList StashNotifier::fileList(const QString &path) //forwards list over QDBus to the KIO worker
{
    QStringList contents;
    StashFileSystem::StashNodeData node = fileSystem->findNode(path);
    if (node.type != StashFileSystem::NodeType::DirectoryNode) {
        contents.append("error::error::InvalidNode");
    } else {
        for (auto it = node.children->begin(); it != node.children->end(); ++it) {
            contents.append(encodeString(it, path));
        }
    }
    return contents;
}

QString StashNotifier::fileInfo(const QString &path) //forwards data of a single file to the KIO worker
{
    QString fileData;
    StashFileSystem::StashNodeData node = fileSystem->findNode(path);
    fileData = encodeString(node, path);
    return fileData;
}

void StashNotifier::addPath(const QString &source, const QString &stashPath, int fileType)
{
    QString processedPath = processString(stashPath);

    if (fileSystem->findNode(stashPath).type == StashFileSystem::NodeType::InvalidNode) {
        if (fileType == StashFileSystem::NodeType::DirectoryNode) {
            dirWatch->addDir(processedPath);
            fileSystem->addFolder(processedPath);
        } else if (fileType == StashFileSystem::NodeType::FileNode) {
            dirWatch->addFile(source);
            fileSystem->addFile(processString(source), stashPath);
        } else if (fileType == StashFileSystem::NodeType::SymlinkNode) {
            dirWatch->addFile(source);
            fileSystem->addSymlink(processString(source), stashPath);
        }
        emit listChanged();
    }
}

QString StashNotifier::processString(const QString &path)
{
    QString processedPath = path.simplified();
    if (processedPath.at(processedPath.size() - 1) == '/') {
        processedPath.chop(1);
    }
    return processedPath;
}

void StashNotifier::removeWatchedPath(const QString &filePath)
{
    qDebug() << filePath;
    QStringList matchedFiles;
    fileSystem->findPathFromSource(filePath, "", matchedFiles, fileSystem->getRoot().children);
    foreach(QString file, matchedFiles) {
        fileSystem->delEntry(file);
    }
}

void StashNotifier::removePath(const QString &path)
{
    QString encodedName = fileInfo(path);
    QString filePath = encodedName.split("::", Qt::KeepEmptyParts).at(2);
    dirWatch->removeFile(filePath);
    fileSystem->delEntry(path);
    emit listChanged();
}

void StashNotifier::nukeStash()
{
    qDebug() << "Nuking stash: all files on it will be deleted!";
    fileSystem->deleteAllItems();
    qDebug() << "Nuked.";
}

void StashNotifier::pingDaemon()
{
    // just to see if this exists in kded5 or whether we need to create a process for it for the test case
}

bool StashNotifier::copyWithStash(const QString &src, const QString &dest)
{
    return fileSystem->copyFile(src, dest);
}

void StashNotifier::dirty(const QString &path)
{
    //nothing to be done here
}

void StashNotifier::created(const QString &path)
{
    //nothing to be done here
}

#include "stashnotifier.moc"
