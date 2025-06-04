## Lab4 实验报告

### 学生信息

姓名：李子沐；学号：221180029；邮箱：221180029@smail.nju.edu.cn

### 实验环境和编译方法

实验环境与OJ要求相同，使用makefile编译即可。
若目录结构为
├── Assembly
│   ├── fib.s
│   ├── ...
├── Code
│   ├── Makefile
│   ├── ...
├── README
├── Test
│   ├── fib.cmm
│   ├── ...
├── parser
├── report.md

可以通过运行下面的命令进行测试：
```shell
./parser ./Test/fib.cmm ./Assembly/fib.s
spim -file ./Assembly/fib.s
```

### 实现功能和细节

完成实验要求（本节没有选做），可以根据前面的中间代码结果，生成MIPS目标汇编指令。

新的数据结构：Register就是代表每个寄存器（实现起来类似ICS的PA），VarOffset本质是由所有变量组成的链表，其每个节点代表了一个变量，name代表变量名，offset代表相对偏移（用于寻址），regNo代表被存储的寄存器编号。
```C
struct Register {
    char *name;
    bool used;
    VarOffset *var;
};

struct VarOffset {
    char name[32];
    int regNo;
    int offset;
    VarOffset *next;
};
```
新的函数：
- `get_reg`：将当前操作数载入到一个合适的寄存器中，对于不同的类型`#1`, `x`, `*x`, `&x`需要考虑不同的指令，例如对于立即数使用`li`，对于访问指针需要先载入地址，然后用寻址，对应两次`lw`.
由于采用朴素寄存器分配，因此只需要遍历即可，例如
```C
for (int i = 8; i < 16; i++) {
    if (regs[i].used == 0) {
        regs[i].used = 1;
        regs[i].var = NULL;
        fprintf(file, "\tli %s, %d\n", regs[i].name, op->value);
        return i;
    }
}
```
- `save2mem`：将当前reg的内容存储到内存中
- `get_varoffset`和`update_varoffset`：获取当前操作数（变量）的偏移量，更新全局维护的变量（偏移量）链表
以及还有一些简单的功能函数，例如`init_all_regs`和`deactivate_temp_regs`初始化所有寄存器和重置所有临时寄存器（t0-t8）

注意记得把lab3的打印中间代码注释掉

### 总结

5月4号写完lab3之后几乎一个月没有碰过`编译原理`，在这一个月里去香港申请了PhD，昨天拿到了梦导的offer，花了两天天时间简单的实现了lab4目标代码生成。

回顾前3次实验，自己一个人做虽然费时间，不过确确实实提高了我的编程水平。当初选这门课的时候，是因为在做一些AI编译器的项目，但是传统的编译架构和AI编译差别比较大哈哈哈（

因为拿到了offer（本身我也是跨专业选），所以后续lab5和考试就60分万岁啦，希望后续有空之后能尝试图染色，窥孔优化等更高级的Compilation-level optimization。

感谢助教学长们的帮助！








