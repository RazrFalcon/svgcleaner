/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2014 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#include <QtGui/QDropEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QHeaderView>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtDebug>

#include "filesview.h"

// TODO: check is path already in tree

// ButtonDelegate class

void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    if (index.parent().isValid())
        return;
    static QPixmap deletePix = QPixmap(":/delete.svgz").scaledToHeight(option.rect.height()-4,
                                                                        Qt::SmoothTransformation);
    int x = option.rect.x() + option.rect.width() - deletePix.width()-2;
    int y = option.rect.y() + option.rect.height() - deletePix.height()-2;
    painter->drawPixmap(x, y, deletePix);
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                 const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (!index.parent().isValid() && event->type() == QEvent::MouseButtonRelease)
        emit removePath(index);
    return QItemDelegate::editorEvent(event, model, option, index);
}

// TreeItem class

TreeItem::TreeItem(const QString &path, TreeItem *parent)
{
    QFileInfo fi(path);
    if (fi.isDir()) {
        name = QDir(path).dirName();
        m_isFolder = true;
    } else if (fi.isFile()) {
        name = fi.fileName();
        m_isFolder = false;
    }
    parentItem = parent;
    itemData = path;
    m_checkState = Qt::Checked;
    m_isEnabled = true;

}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

bool TreeItem::appendChild(TreeItem *item)
{
    childItems.append(item);
    return true;
}

bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool TreeItem::hasChildren()
{
    return !childItems.isEmpty();
}

bool TreeItem::isFolder()
{
    return m_isFolder;
}

TreeItem *TreeItem::child(int row)
{
    return childItems.value(row);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::columnCount() const
{
    return 2;
}

QString TreeItem::data() const
{
    return itemData;
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

Qt::CheckState TreeItem::checkState()
{
    return m_checkState;
}

void TreeItem::setCheckState(Qt::CheckState state)
{
    m_checkState = state;
}

QList<TreeItem *> TreeItem::childList()
{
    return childItems;
}

QString TreeItem::itemName() const
{
    return name;
}

Qt::ItemFlags TreeItem::flags() const
{
    Qt::ItemFlags currFlags = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    if (!m_isEnabled)
        currFlags &= ~(Qt::ItemIsEnabled);
    return currFlags;
}

void TreeItem::setEnabled(bool flag)
{
    m_isEnabled = flag;
}

int TreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

// TreeModel class

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    int iconSize = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize);
    rootItem = new TreeItem("Files");
    m_pixFolder = QPixmap(":/open.svgz").scaled(iconSize, iconSize,
                                                Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_pixFile = QPixmap(":/svg-file.svgz").scaled(iconSize, iconSize,
                                                  Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem;
}

bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    beginRemoveRows(parent, row, row + count - 1);
    bool success = parentItem->removeChildren(row, count);
    endRemoveRows();
    return success;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (   role != Qt::DisplayRole && role != Qt::EditRole
        && role != Qt::CheckStateRole && role != Qt::DecorationRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (role == Qt::DecorationRole) {
        if (item->hasChildren())
            return m_pixFolder;
        else
            return m_pixFile;
    }
    if (role == Qt::CheckStateRole)
        return item->checkState();
    if (role == Qt::EditRole || !index.parent().isValid())
        return item->data();

    return item->itemName();
}

void TreeModel::setCheckToChildren(Qt::CheckState state, TreeItem *parent)
{
    foreach (TreeItem *child, parent->childList()) {
        child->setCheckState(state);
        child->setEnabled(state == Qt::Checked);
        if (child->columnCount() > 0)
            setCheckToChildren(state, child);
    }
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.column() == 0) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (value.toInt() == 0) {
            item->setCheckState(Qt::Unchecked);
            setCheckToChildren(Qt::Unchecked, item);
        } else {
            item->setCheckState(Qt::Checked);
            setCheckToChildren(Qt::Checked, item);
        }
    }
    emit dataChanged(QModelIndex(), QModelIndex());
    return true;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    return item->flags();
}

QVariant TreeModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

TreeItem* TreeModel::addFolder(const QString &text, TreeItem *parent)
{
    if (!parent)
        parent = rootItem;
    TreeItem *folderItem = new TreeItem(text, parent);
    parent->appendChild(folderItem);
    reset();
    return folderItem;
}

void TreeModel::addFile(const QString &text, TreeItem *parent)
{
    if (!parent)
        parent = rootItem;
    parent->appendChild(new TreeItem(text, parent));
    reset();
}

void TreeModel::removeRootItem(const QModelIndex &itemIndex)
{
    if (!itemIndex.isValid())
        return;
    removeRow(itemIndex.row(), itemIndex.parent());
}

// FilesView class

FilesView::FilesView(QWidget *parent) :
    QTreeView(parent)
{
    m_isRecursive = false;
    setAcceptDrops(true);
    m_model = new TreeModel(this);
    setModel(m_model);
    ButtonDelegate *delegate = new ButtonDelegate(this);
    connect(delegate, SIGNAL(removePath(QModelIndex)), this, SLOT(onRemovePath(QModelIndex)));
    setItemDelegateForColumn(1, delegate);
    header()->setResizeMode(0, QHeaderView::Stretch);
    header()->setResizeMode(1, QHeaderView::Fixed);
    header()->setDefaultSectionSize(header()->minimumSectionSize());
}

quint32 FilesView::scanFolder(const QString &path, TreeModel *model, TreeItem *parent)
{
    quint32 filesCount = 0;
    QDir dir(path);
    if (!dir.exists())
        return filesCount;
    static const QStringList filesFilter = QStringList() << "*.svg" << "*.svgz";
    QStringList fileList = dir.entryList(filesFilter,  QDir::Files, QDir::Name);
    QStringList folderList = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
    TreeItem *rootFolder = 0;
    if (!fileList.isEmpty() || !folderList.isEmpty()) {
        if (!parent)
            rootFolder = model->addFolder(path);
        else
            rootFolder = model->addFolder(QDir(path).path(), parent);
    }

    if (m_isRecursive && !folderList.isEmpty()) {
        foreach (const QString &subdir, folderList)
            filesCount += scanFolder(path + "/" + subdir, model, rootFolder);
    }
    if (!fileList.isEmpty()) {
        foreach (const QString &file, fileList) {
            model->addFile(QDir(path).absoluteFilePath(file), rootFolder);
            filesCount++;
        }
    }
    return filesCount;
}

void FilesView::onRemovePath(const QModelIndex &index)
{
    m_rootPaths.removeAt(index.row());
    m_model->removeRootItem(index);
}

bool FilesView::scanModel(const QModelIndex &parent)
{
    // remove folders without svg files in it
    bool anyRemoved = false;
    for (int row = 0; row < m_model->rowCount(parent); ++row) {
        TreeItem *item = m_model->getItem(m_model->index(row, 0, parent));
        QModelIndex index = m_model->index(row,0, parent);
        if (item->hasChildren()) {
            if (scanModel(index)) {
                scanModel(parent);
            }
        } else if (item->isFolder()) {
            m_model->removeRow(row, parent);
            anyRemoved = true;
            scanModel(parent);
        }
    }
    return anyRemoved;
}

void FilesView::addRootPath(const QString &path, bool isFromGui)
{
    if (QFileInfo(path).isDir()) {
        if (isFolderExistInTree(path)) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Selected folder is already exist in folder tree."));
            return;
        }
        quint32 filesAdded = scanFolder(path, m_model);
        scanModel();
        if (isFromGui) {
            if (filesAdded == 0) {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Selected folder does not contains any svg(z) files."));
                return;
            }
        }
    } else {
        if (QFile(path).exists()) {
            if (isFileExistInTree(path)) {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Selected file is already exist in files tree."));
                return;
            }
            m_model->addFile(path);
        } else {
            return;
        }
    }
    m_rootPaths << path;
    m_lastPath = path;
}

void FilesView::setRecursive(bool flag)
{
    m_isRecursive = flag;
    QStringList list = m_rootPaths;
    clear();
    m_rootPaths.clear();
    foreach (const QString &path, list)
        addRootPath(path, false);
}

int FilesView::filesCount(const QModelIndex &parent)
{
    int count = 0;
    for (int row = 0; row < m_model->rowCount(parent); ++row) {
        TreeItem *item = m_model->getItem(m_model->index(row, 0, parent));
        QModelIndex index = m_model->index(row, 0, parent);
        if (item->hasChildren())
            count += filesCount(index);
        else if (item->checkState() == Qt::Checked && !item->isFolder())
            count++;
    }
    return count;
}

bool FilesView::isFolderExistInTree(const QString &path, const QModelIndex &parent)
{
    bool exist = false;
    for (int row = 0; row < m_model->rowCount(parent); ++row) {
        TreeItem *item = m_model->getItem(m_model->index(row, 0, parent));
        QModelIndex index = m_model->index(row,0, parent);
        if (item->isFolder()) {
            if (item->data() == path)
                exist = true;
            else
                exist = isFolderExistInTree(path, index);
        }
        if (exist)
            break;
    }
    return exist;
}

bool FilesView::isFileExistInTree(const QString &path, const QModelIndex &parent)
{
    bool exist = false;
    for (int row = 0; row < m_model->rowCount(parent); ++row) {
        TreeItem *item = m_model->getItem(m_model->index(row, 0, parent));
        QModelIndex index = m_model->index(row,0, parent);
        if (item->isFolder()) {
            exist = isFileExistInTree(path, index);
        } else {
            if (item->data() == path)
                exist = true;
        }
        if (exist)
            break;
    }
    return exist;
}

bool FilesView::hasFiles(const QModelIndex &parent)
{
    return filesCount(parent) > 0;
}

QStringList FilesView::rootList()
{
    QStringList list;
    for (int row = 0; row < m_model->rowCount(QModelIndex()); ++row) {
        TreeItem *item = m_model->getItem(m_model->index(row, 0, QModelIndex()));
        list << item->data();
    }
    return list;
}

QStringList FilesView::rootFiles(const QString &path)
{
    QStringList list;
    for (int row = 0; row < m_model->rowCount(QModelIndex()); ++row) {
        TreeItem *item = m_model->getItem(m_model->index(row, 0, QModelIndex()));
        if (item->data() == path && item->checkState() == Qt::Checked) {
            QModelIndex index = m_model->index(row, 0, QModelIndex());
            if (item->hasChildren())
                list = fileList(index);
            else
                list << item->data();
            break;
        }
    }
    return list;
}

QStringList FilesView::fileList(const QModelIndex &parent)
{
    QStringList list;
    for (int row = 0; row < m_model->rowCount(parent); ++row) {
        TreeItem *item = m_model->getItem(m_model->index(row, 0, parent));
        QModelIndex index = m_model->index(row, 0, parent);
        if (item->hasChildren())
            list << fileList(index);
        else if (item->checkState() == Qt::Checked)
            list << item->data();
    }
    return list;
}

QString FilesView::lastPath()
{
    return m_lastPath;
}

void FilesView::clear(bool clearPathList)
{
    if (clearPathList)
        m_rootPaths.clear();
    while (m_model->rowCount() > 0)
        m_model->removeRootItem(m_model->index(0,0));
}

void FilesView::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void FilesView::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void FilesView::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    if (!mime->hasUrls()) {
        event->ignore();
        return;
    }

    foreach (const QUrl &url, mime->urls()) {
        if (url.isLocalFile()) {
            QString path = url.toLocalFile();
            if (QFileInfo(path).isDir()) {
                addRootPath(path);
            } else if (QFileInfo(path).isFile()) {
                QString suffix = QFileInfo(path).suffix().toLower();
                if (suffix == "svg" || suffix == "svgz")
                    addRootPath(path);
                else
                    QMessageBox::warning(this, tr("Warning"),
                                         tr("You can drop only svg(z) files or folders."));
            }
        }
    }
    event->acceptProposedAction();
}
