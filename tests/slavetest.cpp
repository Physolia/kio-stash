#include "slavetest.h"
#include "../src/ioslave/ioslave.h"

#include <QTest>
#include <QString>
#include <QDir>
#include <QFile>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDBusMessage>
#include <QDBusConnection>

#include <KIO/Job>
#include <KIO/CopyJob>
#include <KIO/DeleteJob>
#include <KFileItem>

void SlaveTest::initTestCase()
{

}

void SlaveTest::cleanupTestCase()
{

}

QString SlaveTest::tmpDirPath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation + QString("/slavetest/"));
}

void SlaveTest::stashFile(const QString &realPath, const QString &stashPath)
{
    //do something naughty and add D-Bus messages to the stashnotifier?
    QDBusMessage msg = QDBusMessage::createMethodCall(
                           "org.kde.kio.StashNotifier", "/StashNotifier", "", "addPath");
    msg << realPath << stashPath << NodeType::FileNode;
    bool queued = QDBusConnection::sessionBus().send(msg);
    QVERIFY(queued);
}

void SlaveTest::stashDirectory(const QString &path)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
                           "org.kde.kio.StashNotifier", "/StashNotifier", "", "addPath");
    QString destinationPath = path;
    msg << "" << destinationPath << NodeType::DirectoryNode;
    bool queued = QDBusConnection::sessionBus().send(msg);
    QVERIFY(queued);
}

void SlaveTest::stashSymlink(const QString &realPath, const QString &stashPath)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
                           "org.kde.kio.StashNotifier", "/StashNotifier", "", "addPath");
    msg << realPath << stashPath << NodeType::SymlinkNode;
    bool queued = QDBusConnection::sessionBus().send(msg);
    QVERIFY(queued);
}

void SlaveTest::statUrl(const QUrl &url, KIO::UDSEntry &entry)
{
    KIO::StatJob *statJob = KIO::stat(url, KIO::HideProgressInfo);
    bool ok = statJob->exec();
    if (ok) {
        entry = statJob->statResult();
    }
    QVERIFY(ok);
}

void SlaveTest::stashCopy(const QUrl &src, const QUrl &dest)
{
    KIO::Job *job = KIO::copyAs(src, dest, KIO::HideProgressInfo);
    bool ok = job->exec();
    QVERIFY(ok);
}

void SlaveTest::moveFromStash(const QUrl &src, const QUrl &dest) //make this work
{
    KIO::Job *job = KIO::moveAs(src, dest, KIO::HideProgressInfo);
    bool ok = job->exec();
    QVERIFY(ok);
    QVERIFY(QFile::exists(dest.toString()));
}

void SlaveTest::deleteFromStash(const QUrl &url)
{
    KIO::Job *delJob = KIO::del(url, KIO::HideProgressInfo);
    bool ok = delJob->exec();
    QVERIFY(ok);
}

void SlaveTest::listRootDir()
{
    //write qcompare cases
    KIO::ListJob *job = KIO::listDir(QUrl(QStringLiteral("stash:/")), KIO::HideProgressInfo);
    connect(job, SIGNAL(entries(KIO::Job*,KIO::UDSEntryList)),
    SLOT(slotEntries(KIO::Job*,KIO::UDSEntryList)));
    bool ok = job->exec();
    QVERIFY(ok);
}

void SlaveTest::listSubDir()
{
    //write qcompare cases
    KIO::ListJob *job = KIO::listDir(QUrl(QStringLiteral("stash:/myfolder")), KIO::HideProgressInfo);
    connect(job, SIGNAL(entries(KIO::Job*,KIO::UDSEntryList)),
            SLOT(slotEntries(KIO::Job*,KIO::UDSEntryList)));
    bool ok = job->exec();
    QVERIFY(ok);
}

void SlaveTest::createDirectory() //find ways for finding files
{
    KIO::SimpleJob *job = KIO::mkdir(url);
    bool ok = job->exec();
    QVERIFY(QFile::exists(url));
    QCOMPARE(ok, true);
}

void SlaveTest::statRoot()
{
    QString url(QStringLiteral("stash:/"));
    KIO::UDSEntry entry;
    statUrl(url, entry);
    KFileItem item(entry, url);
    QVERIFY(item.isDir());
    QVERIFY(!item.isLink());
    QVERIFY(item.isReadable());
    QVERIFY(item.isWritable());
    QVERIFY(!item.isHidden());
    QCOMPARE(item.name(), QStringLiteral("."));
}

void SlaveTest::statFileInRoot()
{
    QFile file
    QUrl url(QStringLiteral("stash:/stashfile"));
    stashFile(url.path(), url.path());
    KIO::UDSEntry entry;
    statUrl(url, entry);
    KFileItem item(entry, url);
    QVERIFY(item.isFile());
    QVERIFY(!item.isDir());
    QVERIFY(!item.isLink());
    QVERIFY(item.isReadable());
    QVERIFY(!item.isWritable());
    QVERIFY(!item.isHidden());
    QCOMPARE(item.text(), QStringLiteral("stashfile"));
}

void SlaveTest::statDirectoryInRoot()
{
    QUrl url(QStringLiteral("stash:/stashfolder"));
    stashDirectory(url.path());
    KIO::UDSEntry entry;
    statUrl(url, entry);
    KFileItem item(entry, url);
    QVERIFY(!item.isFile());
    QVERIFY(item.isDir());
    QVERIFY(!item.isLink());
    QVERIFY(item.isReadable());
    QVERIFY(!item.isWritable());
    QVERIFY(!item.isHidden());
    QCOMPARE(item.text(), QStringLiteral("stashfolder"));
}

void SlaveTest::statSymlinkInRoot()
{
    QUrl url(QStringLiteral("stash:/stashsymlink"));
    stashSymlink(url.path(), url.path());
    KIO::UDSEntry entry;
    statUrl(url, entry);
    KFileItem item(entry, url);
    QVERIFY(!item.isFile());
    QVERIFY(!item.isDir());
    QVERIFY(item.isLink());
    QVERIFY(item.isReadable());
    QVERIFY(!item.isWritable());
    QVERIFY(!item.isHidden());
    QCOMPARE(item.text(), QStringLiteral("stashsymlink"));
}

void SlaveTest::statFileInDirectory()
{
    QUrl url(QStringLiteral("stash:/stashtestfolder/testfile"));
    stashFile(url.path(), url.path());
    KIO::UDSEntry entry;
    statUrl(url, entry);
    KFileItem item(entry, url);
    QVERIFY(item.isFile());
    QVERIFY(!item.isDir());
    QVERIFY(!item.isLink());
    QVERIFY(item.isReadable());
    QVERIFY(!item.isWritable());
    QVERIFY(!item.isHidden());
    QCOMPARE(item.text(), QStringLiteral("testfile"));

}

void SlaveTest::copyFileToStash()
{
    QString src(tmpDirPath() + "stashTestFile");
    QFile stashFile(src);
    QVERIFY(stashFile.open(QIODevice::WriteOnly));
    QString dest("stash:/");
    destinationFileName = src.fileName();
    stashCopy(src, dest);
    QVERIFY(stashFile.exists());
    QVERIFY(dest + destinationFileName);

    QString destDirectory("stash:/copyTestCase");
    destinationFileName = src.fileName();
    stashCopy(src, destDirectory);
    QVERIFY(stashFile.exists());
    QVERIFY(destDirectory + destinationFileName);
}

void SlaveTest::copySymlinkFromStash() //create test case
{
    QString src("stash:/");
    QString dest("file:/");
    stashCopy(src, dest);
    QVERIFY(QFile(dest).exists());
    QCOMPARE(src.fileName(), dest.fileName());
}

void SlaveTest::copyStashToFile()
{
    QString src("stash:/stashTestFile");
    QString dest(tmpDirPath());
    destinationFileName = src.fileName();
    stashCopy(src, dest);
    QVERIFY(QFile(dest).exists());
    QCOMPARE(src.fileName(), dest.fileName());
}

void SlaveTest::copyStashToStash()
{
    QString src("stash:/");
    QString dest("file:/");
    stashCopy(src, dest);
    QVERIFY(QFile(dest).exists());
    QCOMPARE(src.fileName(), dest.fileName());

}

void SlaveTest::moveToFileFromStash()
{
    QString src("stash:/");
    QString dest("file:/");
    moveFromStash(src, dest);
    QVERIFY(QFile(dest.toString()).exists());
    QVERIFY(!QFile(src.toString()).exists());
    //match properties also
}

void SlaveTest::moveToStashFromStash()
{
    QString src("stash:/");
    QString dest("stash:/");
    moveFromStash(src, dest);
    QVERIFY(QFile(dest.toString()).exists());
    QVERIFY(!QFile(src.toString()).exists());
}

void SlaveTest::delRootFile()
{
    QString url("");
    deleteFromStash(url);
    QCOMPARE(QFile(url.toString()).exists(), false);
}

void SlaveTest::delFileInDirectory()
{
    QString url("");
    deleteFromStash(url);
    QCOMPARE(QFile(url.toString()).exists(), false);
}

void SlaveTest::delDirectory()
{
    QString url("");
    deleteFromStash(url);
    QCOMPARE(QFile(url.toString()).exists(), false);
}

QTEST_MAIN(SlaveTest)
#include "slavetest.moc"
