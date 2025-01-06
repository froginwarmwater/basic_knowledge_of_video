# MacOS 安装ffmpeg

课程上的是4.3版本的ffmpeg,进入官网下面有选择发行版本的按钮,安装对应版本
在课程的基础上,需要加上 --disable-x86asm
--disable-static  关闭静态库
--enable-shared   打开动态库

./configure --prefix=/usr/local/ffmpeg --enable-debug=3 --disable-static --enable-shared --disable-x86asm
make -j 4
make install