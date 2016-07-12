#ifndef STASHFS_H
#define STASHFS_H

#include <QObject>
#include <QPointer>
#include <QHash>
#include <QString>
#include <QDebug>

class StashFileSystem : public QObject
{
    Q_OBJECT

public:

    enum NodeType {
        DirectoryNode,
        SymlinkNode,
        FileNode,
        InvalidNode
    };

    struct StashNodeData;

    typedef QHash<QString, StashNodeData> StashNode;

    struct StashNodeData
    {
        StashNodeData(NodeType ntype) :
            type(ntype),
            children(nullptr)
        {}

        NodeType type;
        QString source;
        StashFileSystem::StashNode *children;
    };

    explicit StashFileSystem(QObject *parent = 0);
    virtual ~StashFileSystem();

    bool delEntry(QString path);
    bool addFile(QString src, QString dest);
    bool addFolder(QString dest);
    bool addSymlink(QString src, QString dest);

    StashNodeData findNode(QString path);
    StashNodeData findNode(QStringList path);

    void displayNode(StashNodeData node) //broken af
    {
        //QString fullpath;
        for (auto it = node.children->begin(); it != node.children->end(); it++) {
            qDebug() << "stashpath" << it.key();
            qDebug() << "filepath" << it.value().source;
            qDebug() << "filetype" << it.value().type;
            if (it.value().type == DirectoryNode) {
                qDebug() << "parent" << it.key();
                displayNode(findNode(it.key()));
            }
        }
        return;
    /*  qDebug() << "displayingdata";
        StashNodeData data = findNode("/kqe/ljkas/ipqw");
        for (auto it = data.children->begin(); it != data.children->end(); it++) {
            qDebug() << it.key();
        }*/
    }

    void displayRoot()
    {
        displayNode(*root);
    }

private:

    bool addNode(QString location, StashNodeData* data);
    void deleteChildren(StashNodeData nodeData);
    QStringList splitPath(QString path);
    StashNodeData *root;
};

#endif
