#ifndef ABOUTOPENGLDIALOG_H
#define ABOUTOPENGLDIALOG_H

#include <QDialog>

namespace Ui {
class AboutOpenGLDialog;
}

class AboutOpenGLDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutOpenGLDialog(QWidget *parent = nullptr);
    ~AboutOpenGLDialog();

    void setContents(const QString &text);

private:
    Ui::AboutOpenGLDialog *ui;
};

#endif // ABOUTOPENGLDIALOG_H
