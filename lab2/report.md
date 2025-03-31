## Lab1 实验报告

### 学生信息

姓名：李子沐；学号：221180029；邮箱：221180029@smail.nju.edu.cn

### 实验环境和编译方法

实验环境与OJ要求相同，即

- Ubuntu 20.04 64bit
- GCC 7.5.0
- Flex 2.6.4
- Bison 3.6.1

使用makefile编译即可。

使用命令编译方法如下：

在目录`李子沐_221180029/Code/`下，输入

```
flex lexical.l
bison -d syntax.y
gcc main.c astnode.c syntax.tab.c -ldl -o parser
```

### 实现功能

完成实验要求的必做和选做部分，即：

过程记录

1. 符号表的选择：立马想到了Python的字典，查询资料后发现是Hash Table，于是便采用了哈希表
2. 数据结构：
   1. 符号表采用哈希表
   2. 函数表和结构表采用链表
   3. 全局管理depth