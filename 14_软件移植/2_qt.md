## 1.下载qt
qt官网：https://www.qt.io/，下载页面https://download.qt.io/选择5.9.0版本下载
qt-everywhere-opensource-src-5.9.0.tar.xz

注意：同时有.tar.xz和zip两个包，linux 使用qt-everywhere-opensource-src-5.9.0.tar.xz，不能在window文件系统中解压，一些软连接可能会损坏

## 2. 解压 
xz -d qt-everywhere-opensource-src-5.9.0.tar.xz
tar -xvf qt-everywhere-opensource-src-5.9.0.tar

## 3. 设定编译链
qtbase/mkspecs/linux-arm-gnueabi-g++/ 目录下更改文件qmake.conf

```
# modifications to g++.conf
QMAKE_CC                = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-gcc
QMAKE_CXX               = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-g++
QMAKE_LINK              = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-g++
QMAKE_LINK_SHLIB        = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-g++

# modifications to linux.conf
QMAKE_AR                = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-ar cqs
QMAKE_OBJCOPY           = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-objcopy
QMAKE_NM                = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-nm -P
QMAKE_STRIP             = /opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-strip
```

注意： bash的路径qt读取不到，所以需要用绝对路径

## 4. 产生makefile
```shell
./configure -prefix /opt/qt5.9.5-a7 -opensource -confirm-license -release -strip -shared -xplatform linux-arm-gnueabi-g++ -optimized-qmake -c++std c++11 -nomake examples -nomake tools -nomake tests -gui -widgets -dbus-runtime --glib=no --iconv=no --pcre=qt -no-openssl --freetype=qt --harfbuzz=qt -no-opengl -linuxfb --xcb=no --libpng=qt --libjpeg=qt --sqlite=qt
```

-prefix /opt/qt5.9.5-a7 是安装目录
-xplatform linux-arm-gnueabi-g++ 是平台，对应qtbase/mkspecs/目录下的文件夹

## 5. 编译
执行 make （可能需要sudo）


## 错误解决

Makefile:671: recipe for target '../../lib/libQt5Location.so.5.9.0' failed

修改文件vim qtlocation/src/location/maps/qgeotiledmapscene.cpp 
```
609 /*********
610                 if (ogl)
611                     static_cast<QSGDefaultImageNode *>(node)->setAnisotropyLevel(QSGTexture::Anisotropy16x);
612 **********/
613                 dirtyBits |= QSGNode::DirtyMaterial;
614             }
615             if (dirtyBits != 0)
616                 node->markDirty(dirtyBits);
617             it++;
618         }
619     }
620 
621     for (const QGeoTileSpec &s : toAdd) {
622         QGeoTileTexture *tileTexture = d->m_textures.value(s).data();
623         if (!tileTexture || tileTexture->image.isNull())
624             continue;
625         QSGImageNode *tileNode = window->createImageNode();
626         // note: setTexture will update coordinates so do it here, before we buildGeometry
627         tileNode->setTexture(textures.value(s));
628         if (d->buildGeometry(s, tileNode, overzooming)
629                 && qgeotiledmapscene_isTileInViewport(tileNode->rect(), root->matrix(), straight)) {
630             if (tileNode->texture()->textureSize().width() > d->m_tileSize * pixelRatio) {
631                 tileNode->setFiltering(QSGTexture::Linear); // with mipmapping QSGTexture::Nearest generates artifacts
632                 tileNode->setMipmapFiltering(QSGTexture::Linear);
633             } else {
634                 tileNode->setFiltering((d->m_linearScaling || overzooming) ? QSGTexture::Linear : QSGTexture::Nearest);
635             }
636 /**********
637             if (ogl)
638                 static_cast<QSGDefaultImageNode *>(tileNode)->setAnisotropyLevel(QSGTexture::Anisotropy16x);
639 **********/

```


## 6. 安装
执行 make install （可能需要sudo）
然后将 /opt目录下成果物拷贝到开发板的/opt下
### 环境变量设置
```
export QTEDIR=/opt/qt5.9.0
export LD_LIBRARY_PATH=/opt/qt5.9.0/lib:$LD_LIBRARY_PATH
#export QT_QPA_GENERIC_PLUGINS=tslib
export QT_QPA_FONTDIR=$QTEDIR/lib/fonts 
export QT_QPA_PLATFORM_PLUGIN_PATH=$QTEDIR/plugins 
export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0:size=480x272:mmSize=480x272:offset=0x0:tty=/dev/tty1
export QT_QPA_FB_TSLIB=1
export LD_PRELOAD=/lib/preloadable_libiconv.so
```

```
##指定Qt lib下的动态库
 export LD_LIBRARY_PATH=/opt/Qt5.9.6/lib
 ##指定Qt下的插件
 export QT_QPA_PLATFORM_PLUGIN_PATH=/opt/Qt5.9.6/plugins
 ##指定屏幕的设备⽂件
 export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0
 ##指定Qt的字体库（可以⾃⾏添加）
 export QT_QPA_FONTDIR=/opt/Qt5.9.6/lib/fonts
```
5.9.0可能没有字库 /opt/qt5.9.0/fonts/, 字库文件为qpf
## 测试程序

### 编译

```
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>

class HelloWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HelloWidget(QWidget *parent = 0);
    ~HelloWidget();
    //label
    QLabel *m_labelInfo;
};

HelloWidget::HelloWidget(QWidget *parent) : QWidget(parent)
{
    resize(300, 200);
    m_labelInfo = new QLabel( tr("<h1>Hello Widget!</h1>"), this );
    m_labelInfo->setGeometry(10, 10, 200, 40);
}

HelloWidget::~HelloWidget()
{
    delete m_labelInfo; m_labelInfo = NULL;
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    HelloWidget hw;
    hw.show();

    return a.exec();
}

```
g++ moc_hellowidget.o hellowidget.o main.o -L"C:\Qt\Qt5.4.0\5.4\mingw491_32\lib" -lQt5Core -lQt5Gui -lQt5Widgets