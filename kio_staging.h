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

#ifndef KIO_STAGING_H
#define KIO_STAGING_H

#include <kio/forwardingslavebase.h>

class Staging : public KIO::ForwardingSlaveBase
{
    Q_OBJECT
public:
protected:
    QList<QUrl> m_List;
public:
    Staging(const QByteArray &pool, const QByteArray &app);
    ~Staging(){};
protected:
    void listDir(const QUrl &url) Q_DECL_OVERRIDE;
    void listRoot();
    void createRootUDSEntry(KIO::UDSEntry &entry, const QString &physicalPath, const QString &displayFileName, const QString &internalFileName);
    void put(const QUrl & url, int permissions, KIO::JobFlags flags) Q_DECL_OVERRIDE;
    //void rename(const QUrl &src, const QUrl &dest, KIO::JobFlags flags) Q_DECL_OVERRIDE;
    bool rewriteUrl(const QUrl &url, QUrl &newUrl) Q_DECL_OVERRIDE;
    bool checkUrl(const QUrl &url);
    void mkdir(const QUrl &url, int permissions) Q_DECL_OVERRIDE;
    void del(const QUrl &url, bool isfile) Q_DECL_OVERRIDE;
    void buildList(const QUrl &url);
    void displayList();
    void copy(const QUrl &src, const QUrl &dest, int permissions, KIO::JobFlags flags) Q_DECL_OVERRIDE;
    int searchList(const QString &string);
    void readListFromFile();
};

#endif
