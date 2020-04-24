#include "texture.h"
#include <QJsonObject>
#include <QVector2D>
#include "rendering/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"


const char *Texture::TypeName = "Texture";


Texture::Texture() :
    tex(QOpenGLTexture::Target2D),
    wrapMode(QOpenGLTexture::WrapMode::Repeat)
{
    image = QImage(1, 1, QImage::Format::Format_RGB888);
    image .setPixelColor(0, 0, QColor::fromRgb(255, 0, 255));
    needsUpdate = true;
}

Texture::~Texture()
{
    clear();
}

void Texture::update()
{
    if (tex.isCreated()) {
        tex.destroy();
    }

    // Create and upload the texture
    tex.create();
    tex.setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    tex.setMagnificationFilter(QOpenGLTexture::Linear);
    tex.setWrapMode(wrapMode);
    if (!image.isNull())
    {
        const bool opengl = true;
        if (opengl)
        {
            QImage flipped = image.mirrored();
            tex.setData(flipped);
        }
        else
        {
            tex.setData(image);
        }
    }
    else if (hdrData != nullptr) // For HDR images
    {
        gl->glBindTexture(GL_TEXTURE_2D, tex.textureId());
        gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, hdrData);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        clear();
    }
}

void Texture::destroy()
{
    tex.destroy();
}

void Texture::bind(unsigned int textureUnit)
{
    tex.bind(textureUnit);
}

void Texture::clear()
{
    image = QImage();

    if (hdrData != nullptr)
    {
        stbi_image_free(hdrData);
        hdrData = nullptr;
    }
}

void Texture::loadTexture(const char *filename)
{
    clear();

    if (stbi_is_hdr(filename))
    {
        stbi_set_flip_vertically_on_load(true);
        hdrData = stbi_loadf(filename, &w, &h, &comp, 0);
    }
    else
    {
        image = QImage(QString::fromLatin1(filename));

        if (image.isNull())
        {
            qDebug("Could not open image %s in Texture::loadTexture()", filename);
            return;
        }

        w = image.width();
        h = image.height();
        comp = image.depth()/8;
    }

    needsUpdate = true;

    filePath = QString::fromLatin1(filename);
}

void Texture::setImage(const QImage &img)
{
    image = img;
    w = image.width();
    h = image.height();
    comp = image.depth()/8;
    needsUpdate = true;
}

void Texture::setWrapMode(QOpenGLTexture::WrapMode wrap)
{
    wrapMode = wrap;
}

int Texture::width() const
{
    return w;
}

int Texture::height() const
{
    return h;
}

QVector2D Texture::size() const
{
    return QVector2D(w, h);
}

void Texture::read(const QJsonObject &json)
{
}

void Texture::write(QJsonObject &json)
{
}
