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

#include "kio_staging.h"

#include <KLocalizedString>
#include <KConfigGroup>

#include <QDebug>
#include <QMimeType>
#include <QMimeDatabase>
#include <QDir>
#include <QUrl>
#include <QIODevice>
#include <QFile>
#include <QCoreApplication>

class KIOPluginForMetaData : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kio.slave.staging" FILE "staging.json")
};

extern "C" {
    int Q_DECL_EXPORT kdemain(int argc, char **argv)
    {
        QCoreApplication app(argc, argv);
        Staging slave(argv[2], argv[3]);
        slave.dispatchLoop();
        return 0;
    }
}

Staging::Staging(const QByteArray &pool, const QByteArray &app) : KIO::ForwardingSlaveBase("staging", pool, app)
{
    readListFromFile();
    stagefilename = "/tmp/staging-files";
}

void Staging::readListFromFile()
{
    QFile file("/tmp/staging-files");
    QString url;
    if (file.open(QIODevice::ReadOnly)) {
        while (!file.atEnd()) {
            url = file.readLine();
            qDebug() << url;
            buildList(QUrl(url));
            }
    } else {
        qDebug() << "I/O ERROR";
    }
        //listRoot();
    displayList();
    file.close();
}

bool Staging::rewriteUrl(const QUrl &url, QUrl &newUrl) //don't fuck around with this
{
    if (url.scheme() != "file") {
        newUrl.setScheme("file");
        newUrl.setPath(url.path());
    } else {
        newUrl = url;
    }
    return true;
}

void Staging::listRoot()
{
    displayList();
    KIO::UDSEntry entry;
    QString fileName;
    QString filePath;
    for (auto listIterator = m_List.begin(); listIterator != m_List.end(); ++listIterator) {
        filePath = listIterator->path();
        fileName = QFileInfo(filePath).fileName();
        qDebug() << fileName;
        entry.clear();
        createRootUDSEntry(entry, filePath, fileName, fileName);
        listEntry(entry);
    }
    entry.clear();
    finished();
}

void Staging::mkdir(const QUrl &url, int permissions) //we don't actually need this, but it helps in testing out
{
    //QUrl mrl = QUrl("file:///home/nic" + url.path());
    qDebug() << "MkDir Path:";
    qDebug() << url.path();
    //KIO::ForwardingSlaveBase::mkdir(mrl, permissions);
    m_List.append(url);
    displayList();
    //finished();
    listRoot();
}

void Staging::createRootUDSEntry( KIO::UDSEntry &entry, const QString &physicalPath, const QString &displayFileName, const QString &internalFileName) //needs a lot of changes imo
{
    QByteArray physicalPath_c = QFile::encodeName(physicalPath);
    QT_STATBUF buff;
    if (QT_LSTAT(physicalPath_c, &buff) == -1) {
        qWarning() << "couldn't stat " << physicalPath;
        error(KIO::ERR_COULD_NOT_READ, physicalPath); //if dir doesn't exist
        return;
    }
    if (S_ISLNK(buff.st_mode)) {
        char buffer2[ 1000 ];
        int n = readlink(physicalPath_c, buffer2, 999);
        if (n != -1) {
            buffer2[ n ] = 0;
        }
        entry.insert(KIO::UDSEntry::UDS_LINK_DEST, QFile::decodeName(buffer2));
    }

    mode_t type = buff.st_mode & S_IFMT; // extract file type
    mode_t access = buff.st_mode & 07777; // extract permissions
    //access &= 07555; // make it readonly?
    Q_ASSERT(!internalFileName.isEmpty());
    qDebug() << "physicalPath " << physicalPath;
    entry.insert(KIO::UDSEntry::UDS_NAME, physicalPath);   // internal filename, like "0-foo"; this is used by the app.
    entry.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, displayFileName);   // user-visible filename, like "foo"
    entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, type);
    QMimeDatabase db;
    QMimeType mt = db.mimeTypeForFile(physicalPath);
    if (mt.isValid()) {
        entry.insert(KIO::UDSEntry::UDS_MIME_TYPE, mt.name());
    }
    entry.insert(KIO::UDSEntry::UDS_ACCESS, access);
    entry.insert(KIO::UDSEntry::UDS_SIZE, buff.st_size);
    entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME, buff.st_mtime);
    entry.insert(KIO::UDSEntry::UDS_ACCESS_TIME, buff.st_atime);
}

void Staging::buildList(const QUrl &url) //just for testing
{
    if(url.path() != ".") {
        QString processedUrl = url.path();
        processedUrl = processedUrl.simplified();
        m_List.append(QUrl(processedUrl));
    }
}

int Staging::searchList(const QString &string) //for Staging::del fxn
{
    int i = 0;
    for (auto it = m_List.begin(); it != m_List.end(); it++, i++) {
        if (it->path() == string) {
            return i;
        }
    }
    return -1;
}

void Staging::del(const QUrl &url, bool isfile) //have some hash/tabulation fxn to serve this and checkUrl; make this more robust
                                                //works as an inital foray into deleting
{
    qDebug() << url.path();
    if (searchList(url.path()) != -1) {
        m_List.removeAt(searchList(url.path()));
        updateFile();
        listRoot();
    } else {
        error(KIO::ERR_CANNOT_READ, url.path());
    }
    //finished();
}

void Staging::updateFile()
{
    QFile file("/tmp/staging-files");
    file.open(QIODevice::WriteOnly | QFile::Text);
    QTextStream out(&file);
    for (auto it = m_List.begin(); it != m_List.end(); it++) {
        out << it->path() << '\n';
    }
}

void Staging::listDir(const QUrl &url) //think a bit about finding a file under a subdir and not
                                        //allowing folders which are parents of a dir
{
    if (url.path() == QString("") || url.path() == QString("/")) {
        listRoot();
        qDebug() << "Rootlist";
        return;
    } else if (checkUrl(url)) {
        QUrl newUrl;
        rewriteUrl(url, newUrl);
        qDebug() << "Good url; FSB called" << newUrl.path();
        KIO::ForwardingSlaveBase::listDir(newUrl);
        return;
    } else {
        error(KIO::ERR_SLAVE_DEFINED, i18n("The URL %1 either does not exist or has not been staged yet", url.path()));
    }
}

void Staging::displayList()
{
    for (auto it = m_List.begin(); it != m_List.end(); it++) {
        qDebug() << it->path();
    }
}

void Staging::copy(const QUrl &src, const QUrl &dest, int permissions, KIO::JobFlags flags)
{
    qDebug() << "COPYING HERE" << src.path();
    if (searchList(src.path()) == -1) {
        m_List.append(src);
    }
    displayList();
    listRoot();
}

void Staging::put(const QUrl & url, int permissions, KIO::JobFlags flags)
{
    /*qDebug() << "LOOK HERE" << url.toLocalFile();
    m_List.append(url);
    displayList();
    finished();*/
    qDebug() << "TO BE SUPPORTED";
    error(KIO::ERR_CANNOT_MKDIR, url.path());
}

/*void Staging::rename(const QUrl &src, const QUrl &dest, KIO::JobFlags flags) //why this no work >:
{
    //KIO::ForwardingSlaveBase::rename(src, dest, flags);
    QUrl _src, _dest;
    rewriteUrl(src, _src);
    rewriteUrl(dest, _dest);
    QFile(_src.path()).rename(_dest.path());
}*/

bool Staging::checkUrl(const QUrl &url) //replace with a more efficient algo later
                                        //use a hashing fxn?
{
    for (auto listIterator = m_List.begin(); listIterator != m_List.end(); listIterator++) {
        if (listIterator->path() == url.path() || url.path().startsWith(listIterator->path())) { //prevents dirs which are not children from being accessed
            return true;
        }
    }
    qDebug() << "Bad Url" << url.path();
    return false;
}

#include "kio_staging.moc"
