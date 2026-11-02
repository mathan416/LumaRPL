// Deterministic RGB565-reference to production RGB332-palette conversion.
#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <cstdio>
int main(int argc,char **argv)
{
    QCoreApplication app(argc,argv);if(argc!=2)return 2;
    QDir directory(argv[1]);int count=0;
    for(const auto &name:directory.entryList({"*.png"},QDir::Files))
    {
        QString path=directory.filePath(name);QImage image(path);if(image.isNull())return 3;
        image=image.convertToFormat(QImage::Format_ARGB32);
        for(int y=0;y<image.height();++y)for(int x=0;x<image.width();++x)
        {
            QRgb p=image.pixel(x,y);unsigned r=((qRed(p)>>5)*255+3)/7,g=((qGreen(p)>>5)*255+3)/7,b=(qBlue(p)>>6)*85;
            image.setPixel(x,y,qRgba(r,g,b,qAlpha(p)));
        }
        if(!image.save(path,"PNG"))return 4;++count;
    }
    std::printf("%d\n",count);return count?0:5;
}
