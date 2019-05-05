# AOS

the Atom Operating System

## 介绍

对川合秀实所著《30天自制操作系统》3~30天的操作系统源码进行修改。

修改部分：

- 对源码结构调整；
- 只保留最后一天的最后一个项目；
- 对源码内容调整，使源码可以在Linux下编译；
- 将注释修改为中文注释；
- 构建Linux下的工具链，这些工具链位于tools文件夹中。

## 环境准备

- Linux
- nasm
- gcc
- qemu
- java

## 运行方法

1. 创建一个新的文件夹；
2. 在根目录下执行`bash configure <空文件夹路径>`命令；
3. 进入`src`目录，运行`make run`命令，程序启动。