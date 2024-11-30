#include "ConnectDialog.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>

ConnectDialog::ConnectDialog(QWidget *parent) :QDialog(parent)
{
    QVBoxLayout *main_l = new QVBoxLayout(this);

    mSsidEdit = new QLineEdit;
    mPassEdit = new QLineEdit;

    QFormLayout *form_l = new QFormLayout;
    form_l->addRow(tr("SSID:"), mSsidEdit);
    form_l->addRow(tr("Password:"), mPassEdit);

    main_l->addLayout(form_l);
    main_l->addStretch();

    QDialogButtonBox *dialogBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    dialogBox->button(QDialogButtonBox::Ok)->setText(tr("Connect"));

    main_l->addWidget(dialogBox);

    connect(dialogBox, &QDialogButtonBox::accepted, this, &ConnectDialog::accept);
    connect(dialogBox, &QDialogButtonBox::rejected, this, &ConnectDialog::reject);
}

QString ConnectDialog::getSsid() const { return mSsidEdit->text();}

QString ConnectDialog::getPassword() const {return mPassEdit->text();}
