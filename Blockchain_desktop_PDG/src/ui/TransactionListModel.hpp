#pragma once
#include <QAbstractListModel>
#include <QString>
#include <QVector>
#define emit

struct TxRow {
    QString txId;
    double amount = 0.0;
    QString currency = "SKBC";
    bool isReceive = true;
    qint64 timestamp = 0;
    int status = 0; // 0=pending, 1=confirmed
};

class TransactionListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { TxIdRole = Qt::UserRole + 1, AmountRole, CurrencyRole, IsReceiveRole, TimestampRole, StatusRole };
    Q_ENUM(Roles)

    explicit TransactionListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex& parent = {}) const override {
        return parent.isValid() ? 0 : m_rows.size();
    }

    QVariant data(const QModelIndex& index, int role) const override {
        if (!index.isValid() || index.row() < 0 || index.row() >= m_rows.size()) return {};
        const auto& r = m_rows[index.row()];
        switch (role) {
        case TxIdRole: return r.txId;
        case AmountRole: return r.amount;
        case CurrencyRole: return r.currency;
        case IsReceiveRole: return r.isReceive;
        case TimestampRole: return r.timestamp;
        case StatusRole: return r.status;
        default: return {};
        }
    }

    QHash<int, QByteArray> roleNames() const override {
        return {
            {TxIdRole, "txId"},
            {AmountRole, "amount"},
            {CurrencyRole, "currency"},
            {IsReceiveRole, "isReceive"},
            {TimestampRole, "timestamp"},
            {StatusRole, "status"}
        };
    }

    Q_INVOKABLE void clear() {
        if (m_rows.isEmpty()) return;
        beginResetModel();
        m_rows.clear();
        endResetModel();
    }

    void append(const TxRow& row) {
        beginInsertRows(QModelIndex(), 0, 0);
        m_rows.prepend(row); // plus récent en haut
        endInsertRows();
    }

    // MAJ statut par txId
    void setConfirmedByTxId(const QString& txId) {
        for (int i = 0; i < m_rows.size(); ++i) {
            if (m_rows[i].txId == txId && m_rows[i].status != 1) {
                m_rows[i].status = 1;
                const QModelIndex idx = index(i);
                emit dataChanged(idx, idx, { StatusRole });
                break;
            }
        }
    }

private:
    QVector<TxRow> m_rows;
};
