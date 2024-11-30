#pragma once

#include <QDialog>

#include <QLineEdit>

class ConnectDialog : public QDialog
{
    Q_OBJECT
public:
    ConnectDialog(QWidget *parent);

    QString getSsid() const;
    QString getPassword()  const;

private:
    QLineEdit *mSsidEdit = nullptr;
    QLineEdit *mPassEdit = nullptr;
};
