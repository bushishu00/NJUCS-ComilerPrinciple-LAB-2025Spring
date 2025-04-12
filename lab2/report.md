## Lab2 实验报告

### 学生信息

姓名：李子沐；学号：221180029；邮箱：221180029@smail.nju.edu.cn

### 实验环境和编译方法

实验环境与OJ要求相同，使用makefile编译即可。

### 实现功能和细节

完成实验要求的必做和选做部分，即：程序可以识别实验指导规定的必做与选做中给出的语义错误
具体来说，我设计了多种数据结构维护整个程序的语义，包括：

1. 基于十字链表SymbolTableNode，在“纵轴”上用HashTable维护，在“横轴”上用ScopeHeadStack维护
2. “纵轴”符号表，为哈希表，“横轴”作用域为栈
3. 基于普通链表的StructureTable和FunctionTable以完成选做要求的：允许函数声明，结构等价

关键的十字链表数据结构如下：

```C
/* a cross linked-list */
/* the Hash table only store the symbol itself.
   but the scope is maintained by the 'nextScopeNode' list*/
struct SymbolTableNode {
    Type *type;                      
    char *name;                     
    int kind;                          // 0 for variable, 1 for struct, 2 for function
    bool isdef;                        // whether the variable is defined
    SymbolTableNode *nextBucketNode;   // next node in the symbol table, linked list insered at the head
    SymbolTableNode *nextScopeNode;    // next node in the same scope
};
struct ScopeHead {
    SymbolTableNode *ScopeHeadNode;    // hash table for this scope
    ScopeHead *nextScope;              // next scope in the stack
};
struct ScopeHeadStack {
    ScopeHead *top;                    // top of the stack
};
/* Hash Table is an array of bucket(pointer) */
struct HashTable {
    SymbolTableNode *buckets[TABLESIZE];
};
```

基于这些数据结构的方法，表类型里实现了必要的`create`、`insert`、`get`，栈类型里实现了`create`、`push`、`pop`。由于删除节点本质上就是一个作用域的终结，因此弹栈也代表着删除符号表的一系列节点。

在实现Compst的块作用域时，本来打算在Compst分析内部进行push和pop，但是考虑到函数体的定义，从形参表开始就属于新的作用域，因此对于Compst的作用域全部在调用前进行push和pop，例如

```C
    scope_begin();// a new scope of FUNCTION begin at Formal Param
    FunDec(node1, TRUE, type);
    CompSt(node2, type);
    scope_end();
```

其余的语义分析，并没有特别的内容，重点是仔细考虑每个细节即可。
实际调试时发现一些问题：

1. 我在lab1中用数组来实现多叉树的多个孩子，因此产生的空串仍然会占用一个，例如A->BCD，如果C产生了空串，那么我仍然要通过访问第三个孩子，才可以获得C，而不是访问第二个孩子
2. 写数据结构的时候留下了很多错误，导致一致segmentation fault，以后还是得先仔细检查数据结构写没写错……
3. 在更多的测试用例上发现，我的lab1存在问题，无法识别if ()else if () ... else的语句，由于测试这些更复杂的用例时已经是周五十点，因此没有修改，后续会对这些问题进行更正。
4. 实验指导的假设要求只有整型才可以参与逻辑运算，我在实现时只要是相同的基础类型就可以运算，并返回对应类型。可以进一步完成该要求，但是由于时间原因，且未作显式的报错规定，因此暂时未考虑。
5. 仍然有一些bug，目前估计是访问结构体的返回类型上出了问题，在简单用例上测试这些错误类型是没问题的，但是涉及复杂一些的程序会有小问题，有待修改。
6. 我使用了全局的structTable来存储定义的结构体标识符，且没有考虑无名结构体的存储，未来有待完善。

### 总结

闭关四天勉勉强强能通过样例测试，最后两小时发现lab1有隐患的时候有点崩溃。代码量非常大，写完真的是收获满满（尽管仍然存在一些bug......



