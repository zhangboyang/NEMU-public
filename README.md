# ZBY 的公开版 PA
仅供参考、对拍使用，抹掉了所有 git log。
当前进度在 PA4，如果要切回之前时间点，把对应的宏、Makefile 改回去即可。


# 计算机系统基础 课程大作业
NEMU 是复旦大学计算机系统基础课的大作业，旨在实现一个教学用的 x86 虚拟机。
通过课程的学习，逐渐向里面添加内容，最终的目标是将仙剑奇侠传 (SDLPAL) 移植进该虚拟机中。

## 高级测试样例
这里的测试样例的全部代码均可以在 https://github.com/zhangboyang/NEMU-testcase 找到。
它们的主要来源如下：
* 数据结构课程作业
* 实现指令时顺手编写的样例
* 遇到的与指令相关的坑

## 针对运行速度的优化
由于要在 NEMU 中运行仙剑奇侠传，因此做了许多速度优化，它们中的大部分仅在 DEBUG 被关闭时才启用。
最终的结果是，在台式 i3 机器上可以流畅运行仙剑奇侠传(此时每秒钟执行约一亿条指令)。
* 关闭 Cache、TLB、DDR 等会拖慢速度的部分
* 启用 VFMEMORY 模块
 * VFMEMORY = very fast memory
 * 几乎完全重写的虚拟机内存模块
 * 一般情况下的内存访问只需两次访存和一次判断和几次位运算
 * 只在段转换的平坦模式下有效
* 直接使用对应的汇编指令计算 EFLAGS，而非用位运算模拟
* 关闭 UI 上的多数功能

## 针对仙剑奇侠传 (SDLPAL) 的修改
为了在 NEMU 中流畅运行仙剑奇侠传，仅仅对 NEMU 优化还不够，还需要对游戏代码本身进行优化。
* 对 ColorShift 操作，由每次判断改为查表，花费少量空间和时间省去内层循环里的判断语句
* 加入专门指令实现高精确度的计时器
 * 废除原有的使用时钟中断作为计时依据的实现
 * 实现专门指令，直接将真机上 clock_gettime() 返回的结果通入虚拟机
 * 由于直接使用 clock_gettime()，计时精度由原本 10ms 增加到微秒级
* 修改了一些遗漏的浮点指令(例如婶婶病倒时背景音乐的切换)
* 一些计时功能(统计函数执行时间用)



## ICS2015 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System) in Department of Computer Science and Technology, NanJing University.

For the guide of this programming assignment, refer to http://nju-ics.gitbooks.io/ics2015-programming-assignment/content/

The following subprojects/components are included. Some of them are not fully implemented.
* NEMU
* testcase
* newlib
* kernel
* typing game
* NEMU-PAL

## NEMU

NEMU(NJU Emulator) is a simple but complete full-system x86 emulator designed for teaching. It is the main part of this programming assignment. Small x86 programs can run under NEMU. The main features of NEMU include
* a small monitor with a simple debugger
 * single step
 * register/memory examination
 * expression evaluation with the support of symbols
 * watch point
 * backtrace
* CPU core with support of most common used x86 instructions in protected mode
 * real mode is not supported
 * x87 floating point instructions are not supported
* DRAM with row buffer and burst
* two-level unified cache
* IA-32 segmentation and paging with TLB
 * protection is not supported
* IA-32 interrupt and exception
 * protection is not supported
* 6 devices
 * timer, keyboard, VGA, serial, IDE, i8259 PIC
 * most of them are simplified and unprogrammable
* 2 types of I/O
 * port-mapped I/O and memory-mapped I/O

## testcase

Some small C programs to test the implementation of NEMU.

## newlib

newlib(https://sourceware.org/newlib) is a C library for embedding systems. It requires minimal run-time support and is very friendly to NEMU.

## kernel

This is the simplified version of Nanos(http://cslab.nju.edu.cn/opsystem). It is a uni-tasking kernel with the following features.
* 2 device drivers
 * Ramdisk
 * IDE
* ELF32 loader
* memory management with paging
* a simple file system
 * with fix number and size of files
 * without directory
* 6 system calls
 * open, read, write, lseek, close, brk

## typing game

This is a fork of the demo of NJU 2013 oslab0(the origin repository has been deleted, but we have a fork of it -- https://github.com/nju-ics/os-lab0). It is ported to NEMU.

## NEMU-PAL

This is a fork of Wei Mingzhi's SDLPAL(https://github.com/CecilHarvey/sdlpal). It is obtained by refactoring the original SDLPAL, as well as porting to NEMU.
