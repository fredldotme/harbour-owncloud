#include "directorycontentmodel.h"

DirectoryContentModel::DirectoryContentModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

/*QVariant DirectoryContentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool DirectoryContentModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}*/

int DirectoryContentModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return this->m_contentList.size();
}

QVariant DirectoryContentModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    if (!index.isValid())
        return QVariant();

    if (index.row() > this->m_contentList.size())
        return QVariant();

    return this->m_contentList.at(index.row());
}

bool DirectoryContentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        this->m_contentList.insert(index.row(), value);
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags DirectoryContentModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    if (index.row() > this->m_contentList.size())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

/*
bool DirectoryContentModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool DirectoryContentModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}
*/
