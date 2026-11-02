// Native-resolution visual review index; uses the simulator's existing Qt dependency.
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QFont>
int main(int argc,char **argv)
{
    QGuiApplication app(argc,argv);if(argc!=3)return 2;
    QDir input(argv[1]),output(argv[2]);output.mkpath(".");
    auto files=input.entryList({"*.ppm"},QDir::Files,QDir::Name);
    QFile index(output.filePath("index.tsv"));if(!index.open(QIODevice::WriteOnly))return 3;QTextStream table(&index);
    QImage sheet;int page=0;
    for(int i=0;i<files.size();++i)
    {
        if(i%20==0){sheet=QImage(1280,5*352,QImage::Format_RGB32);sheet.fill(Qt::white);++page;}
        QImage image(input.filePath(files[i]));if(image.size()!=QSize(320,320))return 4;
        QString name=QFileInfo(files[i]).completeBaseName()+".png";
        QImage previous(output.filePath(name));
        if((previous.isNull() || previous.convertToFormat(QImage::Format_RGB888)!=image.convertToFormat(QImage::Format_RGB888)) && !image.save(output.filePath(name)))return 5;
        QPainter paint(&sheet);paint.setFont(QFont("Menlo",8));paint.setPen(Qt::black);
        int x=i%4*320,y=(i%20)/4*352;paint.drawImage(x,y,image);
        // Two lines keep stable IDs accessible without reducing the actual screenshot.
        QString caption=QFileInfo(files[i]).completeBaseName();paint.drawText(x+2,y+322,316,30,Qt::TextWrapAnywhere,caption);paint.end();
        table<<QString("sheet-%1.png").arg(page,2,10,QChar('0'))<<'\t'<<name<<'\n';
        if(i%20==19||i+1==files.size())if(!sheet.save(output.filePath(QString("sheet-%1.png").arg(page,2,10,QChar('0')))))return 6;
    }
    return 0;
}
