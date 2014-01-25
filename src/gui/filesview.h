#ifndef FILESVIEW_H
#define FILESVIEW_H

#include <QtGui/QTreeView>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QList>

#include <QtGui/QItemDelegate>

class ButtonDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit ButtonDelegate(QObject *parent = 0) : QItemDelegate(parent) {}

signals:
    void removePath(QModelIndex);

private:
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index);
};

class TreeItem
{
public:
    TreeItem(const QString &path, TreeItem *parent = 0);
    ~TreeItem();
    int childCount() const;
    int columnCount() const;
    int row() const;
    QList<TreeItem*> childList();
    QString itemName() const;
    QString data() const;
    Qt::CheckState checkState();
    Qt::ItemFlags flags() const;
    TreeItem *child(int row);
    TreeItem *parent();
    bool appendChild(TreeItem *child);
    void setCheckState(Qt::CheckState state);
    void setEnabled(bool flag);
    bool removeChildren(int position, int count);
    bool hasChildren();
    bool isFolder();

private:
    bool m_isFolder;
    bool m_isEnabled;
    QList<TreeItem*> childItems;
    QString itemData;
    QString name;
    Qt::CheckState m_checkState;
    TreeItem *parentItem;
};

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(QObject *parent = 0);
    ~TreeModel();
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void addFile(const QString &text, TreeItem *parent = 0);
    TreeItem* addFolder(const QString &text, TreeItem *parent = 0);
    void removeRootItem(const QModelIndex &itemIndex);
    TreeItem* getItem(const QModelIndex &index) const;

private:
    TreeItem *rootItem;
    QPixmap m_pixFolder;
    QPixmap m_pixFile;

    void setCheckToChildren(Qt::CheckState state, TreeItem *parent);
};

class FilesView : public QTreeView
{
    Q_OBJECT

public:
    explicit FilesView(QWidget *parent = 0);
    void addRootPath(const QString &path);
    bool hasFiles(const QModelIndex &parent = QModelIndex());
    QStringList rootList();
    QStringList rootFiles(const QString &path);
    QStringList fileList(const QModelIndex &parent = QModelIndex());
    QString lastPath();
    void clear(bool clearPathList = false);

public slots:
    void setRecursive(bool flag);

private:
    bool m_isRecursive;
    TreeModel *m_model;
    QString m_lastPath;
    QStringList m_rootPaths;

    void scanFolder(const QString &path, TreeModel *model, TreeItem *parent = 0);
    void scanModel(const QModelIndex &parent = QModelIndex());

private slots:
    void onRemovePath(const QModelIndex &index);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
};

#endif // FILESVIEW_H
