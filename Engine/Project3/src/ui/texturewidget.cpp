#include "texturewidget.h"
#include "ui_texturewidget.h"
#include "resources/texture.h"
#include <QFileDialog>


TextureWidget::TextureWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextureWidget)
{
    ui->setupUi(this);
    connect(ui->buttonOpen, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
}

TextureWidget::~TextureWidget()
{
    delete ui;
}

void TextureWidget::setTexture(Texture *t)
{
    texture = t;

    if (texture == nullptr) return;

    QFileInfo fileInfo(t->getFilePath());
    ui->textFile->setText(fileInfo.baseName());
    ui->openGLWidget->setTexture(t);
}

void TextureWidget::onButtonClicked()
{
    if (texture == nullptr) return;

    QFileInfo pathInfo(texture->getFilePath());
    QString directory = pathInfo.path();
    QString path = QFileDialog::getOpenFileName(this,"Load image file", directory, QString::fromLatin1("Image files (*.png *.jpg *.gif *.bmp)"));
    if (!path.isEmpty())
    {
        QFileInfo fileInfo(path);
        ui->textFile->setText(fileInfo.baseName());
        texture->loadTexture(path.toLatin1());
        emit resourceChanged(texture);
    }
}
