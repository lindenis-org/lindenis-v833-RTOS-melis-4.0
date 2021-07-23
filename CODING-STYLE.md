# Melis3.0 编程风格

这是一份 Melis3.0 开发人员的开发指引。开发人员需要遵照执行.

## 1.目录名称

目录名称如果无特殊的需求，请使用全小写的形式；目录名称应能够反应部分的意思.
## 2.文件名称

文件名称如果无特殊的需求(如果是引用其他地方，可以保留相应的名称)，请使用全小写
的形式。另外为了避免文件名重名的问题，一些地方请尽量不要使用通用化、使用频率高
的名称。

## 3.头文件定义

C语言头文件为了避免多次重复包含，需要定义一个符号。这个符号的定义形式请采用如下
的风格：

```c
    #ifndef __FILE_H__
    #define __FILE_H__
    /* header file content */
    #endif
```

即定义的符号两侧采用 "__" 以避免重名，另外也可以根据文件名中是否包含多个词语而
采用 "_" 连接起来。

## 4.文件头注释

在每个源文件文件头上，应该包括相应的版权信息，Change Log 记录, 编译器，作者，最后更新时间等信息.

```c
                                                                                                                                                                                              
/*
 * =================================================================================
 *
 *       Filename:  vector_c.c
 *
 *    Description:  rotuine to dealt with exception. based on ARM926EJ-S
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 13:43:10
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-10-19 13:15:07
 *
 * =================================================================================
 */

```

例如采用如上的形式。

## 5.结构体定义

结构体名称请使用小写英文名的形式，单词与单词之间采用 "_" 连接，例如：

```c
    struct list_node
    {
        struct list_node *next;
        struct list_node *prev;
    };
```

其中，"{"，"}" 独立占用一行，后面的成员定义使用缩进的方式定义。

结构体等的类型定义请以结构体名称加上 "_t" 的形式作为名称，例如：

```c
    typedef struct list_node list_t;
```

因为内核中对象引用方便的缘故，采用了对象内核指针作为类型定义的形式，例如：

```c
    typedef struct timer* timer_t;
```

## 6.宏定义

在Melis3.0中，请使用大写英文名称作为宏定义，单词之间使用 "_" 连接，例如：

```c
    #define TRUE                         1
```

## 7.函数名称、声明

函数名称请使用小写英文的形式，单词之间使用 "_" 连接。提供给上层应用使用的 API
接口，必须在相应的头文件中声明；如果函数入口参数是空，必须使用 void 作为入口参
数，例如：

```c
    thread_t thread_self(void);
```

## 8.注释编写

请使用英文做为注释，使用中文注释将意味着在编写代码时需要来回不停的切换中英文输
入法从而打断编写代码的思路。并且使用英文注释也能够比较好的与中国以外的技术者进
行交流。

源代码的注释不应该过多，更多的说明应该是代码做了什么，仅当个别关键点才需要一些
相应提示性的注释以解释一段复杂的算法它是如何工作的。对语句的注释只能写在它的上
方或右方，其他位置都是非法的。

## 9.缩进及分行

缩进请采用 4 个空格的方式。如果没有什么特殊意义，请在 "{" 后进行分行，并在下一
行都采用缩进的方式，例如：

```c
    if (condition)
    {
        /* others */
    }
```

case 语句与前面的 switch 语句对齐，后续的语句则采用缩进的方式。

分行上，如果没有什么特殊考虑，请**不要在代码中连续使用两个以上的空行**。

## 10.大括号与空格

从代码阅读角度，建议每个大括号单独占用一行，而不是跟在语句的后面，例如：

```c
    if (condition)
    {
        /* others */
    }
```

匹配的大括号单独占用一行，代码阅读起来就会有相应的层次而不会容易出现混淆的情况。

空格建议在非函数方式的括号调用前留一个空格以和前面的进行区分，例如：

```c
    if (x <= y)
    {
        /* others */
    }

    for (index = 0; index < MAX_NUMBER; index ++)
    {
        /* others */
    }
```

建议在括号前留出一个空格(涉及的包括 if、for、while、swtich 语句)，而运算表达式
中，运算符与字符串间留一个空格。另外，不要在括号的表达式两侧留空格，例如：

```c
    if ( x <= y )
    {
        /* other */
    }
```

这样括号内两侧的空格是不允许的。

## 11.trace、log信息

在 Melis3.0 中，普遍使用的 log 方式是 __log。__log 在 Melis3.0 被实
现成一个采用轮询、非中断方式的字串输出，能够适合于在中断这类"即时"显示日志的场
合。因为这种轮询方式的存在，也必然会影响到日志输出的时序关系。

建议在代码中不要频繁的使用 __log 作为日志输出，除非你真正的明白，你的代码
运行占用的时间多一些也没什么关系。

日志输出应该被设计成正常情况下是关闭状态(例如通过一个变量或宏就能够开启)，并且
当真正输出日志时，日志是易懂易定位问题的方式。"天书式"的日志系统是糟糕的，不合
理的。

## 12.函数

在内核编程中，函数应该尽量精简，仅完成相对独立的简单功能。函数的实现不应该太长
，函数实现太长，应该反思能够如何修改(或拆分)使得函数更为精简、易懂。

## 13.对象

Melis3.0 内核采用了 C 语言对象化技术，命名表现形式是：对象名结构体表示类定义、
对象名 + 动词短语形式表示类方法，例如：

```c
    struct timer
    {
        struct object parent;
        /* other fields */
    };
    typedef struct timer* timer_t;
```

结构体定义 timer 代表了 timer 对象的类定义；

```c
    timer_t timer_create(const char* name,
    void (*timeout)(void* parameter), void* parameter,
    tick_t time, uint8_t flag);
    err_t timer_delete(timer_t timer);
    err_t timer_start(timer_t timer);
    err_t timer_stop(timer_t timer);
```

timer + 动词短语的形式表示能够应用于 timer 对象的方法。

在创建一个新的对象时，应该思考好，对象的内存操作处理：是否允许一个静态对象存在
，或仅仅支持从堆中动态分配的对象。

## 14. astyle 自动格式化代码
-   源码格式化命令参数：
    astyle --style=allman --indent=spaces=4 --indent-switches --indent-namespaces --indent-col1-comments  --indent-classes --pad-oper --pad-header --unpad-paren --suffix=none  --align-pointer=name --lineend=linux  --convert-tabs  --recursive --verbose --add-brackets --formatted ./*.c
    
-   头文件格式化命令参数:
    astyle --style=allman --indent=spaces=4 --indent-switches --indent-namespaces --indent-col1-comments  --indent-classes  --pad-oper --pad-header --unpad-paren --suffix=none  --align-pointer=name --lineend=linux  --convert-tabs --recursive --verbose --add-brackets  --formatted ./*.h
