# spark sdk 快速上手指南
## 目录结构
    apps ：下分多个应用，可独立与底层链接，形成单独的应用程序镜像，编译核心也是围绕apps里面具体的一个app应用展开的。
    bootloader: 启动镜像目录
    build:  编译规则，链接规则的脚本以及编译器。
    configs ：安装包，向导程序。
    inc ：系统除apps以外的任何包含头文件
    middleware 中间件接口
    hal 系统硬件HAL层接口
    oal 操作系统内核以及OAL层接口
    sys 系统底层相关接口
    gui 图形界面接口
    ..  等等其他库的接口
    libs ：预定或不开源的.a 库文件，或由src编译获得又不开源的部分
    src：（这个目录在发布时，假如不想对外开放，则会屏蔽）
    tools: 固件烧录和调参工具，调试工具

## 环境安装
    configs/develop_install
    windows端安装python-3.6.8-amd64.exe
    Scons/install.bat

## 编译app
    举例：在windows命令行（cmd）：
    cd spark_sdk\apps\yp3\gcc
    scons -s
    编译顺利不出错会在.out文件夹下生成 yp3.up

## 烧录和调试
    按照实际需求安装：
    - SetupFlashloaderSL-FAC(5.5) 工厂量产软件，提供在烧录机架上批量生产烧录机器。
    - SetupFlashloaderSL-DEV(5.5) 研发调试软件，提供最全的研发固件调试，烧录，修改工具。
    - SetupFlashloaderSL-USR(5.5) 用户烧录固件软件，仅仅提供指定固件烧录功能。
    烧录.up文件，利用串口工具调试。
    烧录方法：按住板子上 PD0 按键不松手，再复位或者上电设备，它会检测到设备盘符然后烧录。
    串口工具一般参数是 8 N 1, 波特率 1500000
