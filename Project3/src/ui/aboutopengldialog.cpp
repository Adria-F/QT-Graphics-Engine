#include "aboutopengldialog.h"
#include "ui_aboutopengldialog.h"

AboutOpenGLDialog::AboutOpenGLDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutOpenGLDialog)
{
    ui->setupUi(this);
}

AboutOpenGLDialog::~AboutOpenGLDialog()
{
    delete ui;
}

void AboutOpenGLDialog::setContents(const QString &text)
{
    ui->text->setText(text);
}
