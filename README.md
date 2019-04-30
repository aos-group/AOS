# AOS

the Atom Operating System

## 介绍

对川合秀实所著《30天自制操作系统》3~30天的操作系统源码进行修改。

修改部分：

- 对源码结构调整；
- 将注释修改为中文注释。

## 环境准备

- Windows
- [工具集(tolset_h)](http://hrb.osask.jp/tolset_h.zip)

## 运行方法

1. 将`tolset_h`目录下的`z_tools`子目录复制到项目根目录下(与`src`目录同级)；
2. 打开`z_tools/haribote/haribote.rul`文件，进行如下修改：
   1. 将第11行修改为：`../../../z_tools/haribote/harilibc.lib;`
   2. 将第12行修改为：`../../../z_tools/haribote/golibc.lib;`
3. 进入`src`目录，进入某一天下的某一个子项目；
4. 运行`!cons_nt.bat`文件；
5. 输入`make run`命令，程序启动。