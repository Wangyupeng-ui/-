# **SYNTAX&ensp;实验报告**&ensp;(57122432王宇鹏)
### &ensp;&ensp;&ensp;*Date：2024-12-05*

## 目录

- [**SYNTAX 实验报告** (57122432王宇鹏)](#syntax实验报告57122432王宇鹏)
    - [   *Date：2024-12-05*](#date2024-12-05)
  - [目录](#目录)
  - [1. 实验目的](#1-实验目的)
  - [2. 实验内容](#2-实验内容)
  - [3. 方案 \& 实现](#3-方案--实现)
    - [3.1 整体设计](#31-整体设计)
  - [4. 主要数据结构](#4-主要数据结构)
    - [4.1 读取 CFG、符号部分](#41-读取-cfg符号部分)
    - [4.2 文件预处理部分](#42-文件预处理部分)
    - [4.3 LR(1) 项](#43-lr1-项)
    - [4.4 LR(1) 项目集族](#44-lr1-项目集族)
    - [4.5 LR(1) 预测分析表](#45-lr1-预测分析表)
  - [5. 核心算法](#5-核心算法)
    - [5.1 读取 CFG、符号部分](#51-读取-cfg符号部分)
    - [5.2 预处理部分](#52-预处理部分)
    - [5.3 构造 FIRST 集](#53-构造-first-集)
    - [5.4 构造 DFA](#54-构造-dfa)
    - [5.5 生成预测分析表](#55-生成预测分析表)
  - [6. 测试用例](#6-测试用例)

## 1. 实验目的

根据本学期所学知识，设计一个语法分析器程序，通过此实验，加深对语法分析原理的理解，和了解编译器的基本实现原理。

## 2. 实验内容

1. 从文件中读取 CFG、以及其中的终结符、非终结符；
2. 由 CFG 生成 LR(1) 项目集族；
3. 构造 LR(1) 分析表；
4. 读入一个序列，使用分析表分析规约过程，对不合规的输入报错。

## 3. 方案 & 实现

### 3.1 整体设计

语法分析器使用 C++ 语言实现，主要分为以下几个部分：将符号拆分存储进行预处理、构造 FIRST 集、构造项目集族、构造 LR(1) 预测分析表、分析输入串。将使用到的函数、成员封装为一个类 `SyntaxParser`，并在 `main` 函数中调用、测试。

## 4. 主要数据结构

### 4.1 读取 CFG、符号部分

```cpp
vector<string> content_;             // 存储从文件中读取的每一行内容（每一行为一个产生式）
unordered_map<string, int> sym2id_;  // 符号到 id 的映射（包含终结/非终结符）
vector<string> id2sym_;              // id 到符号的映射
int eps_idx_;                        // ε 在符号表中的下标（即 ε 的 id）
int sym_num_;                        // 符号总数
```

### 4.2 文件预处理部分

```cpp
vector<vector<int>> productions_;  // 将 content_ 中的每一行产生式拆分，并使用 id 存储（左符号，右符号1，右符号2···）
vector<vector<int>> idx_table_;    // 存储每个非终结符出现在左侧的产生式的下标（如 idx_table_[5] = {1,3,4} 表示符号5出现在产生式1、3、4的左侧）
```

### 4.3 LR(1) 项

```cpp
struct Item
{
  int prod_idx = 0;              // 产生式编号
  int dot_pos = 1;               // 点的位置
  unordered_set<int> first_set;  // FIRST 集
};
```

### 4.4 LR(1) 项目集族

```cpp
struct Edge
{
  int next;
  int to;
  int w;
};

vector<vector<Item>> item_set_collection_;  // 项目集族
vector<Edge> edge_set_;                     // 使用有向边存储 DFA 图
unordered_map<int, int> head_;              // 存储每个项目集的头结点
```

### 4.5 LR(1) 预测分析表

```cpp
vector<vector<int>> parsing_table_;  // 预测分析表
vector<vector<int>> sft_rdc_;        // 存储移进/规约状态
```

## 5. 核心算法

### 5.1 读取 CFG、符号部分

要求用户输入 CFG 文件路径，从文件中逐行读取产生式，存储到 `content_`；用户输入终结符、非终结符，将其存储到 `sym2id_`、`id2sym_` 中，同时将终结符、非终结符的数量存储到 `sym_num_` 中。

```cpp
int ReadIn(const string& terminal, const string& nonterminal, const string& file_path);
```

具体实现较为简单，略。

### 5.2 预处理部分

将 `content_` 中的每一行产生式拆分并去除多余空白符，并使用 id 存储，存储到 `productions_` 中；同时将每个非终结符出现在左侧的产生式的下标存储到 `idx_table_` 中。

```cpp
void Preprocess();
vector<string> Split(const string& str, const string& delim);
void RemoveBlanks(string& str);
```

具体实现较为简单，略。

### 5.3 构造 FIRST 集

根据符号 id 和所在产生式的编号，计算 FIRST 集，并存储到 `first_sets_` 中。`GetAllFirstSets()` 负责遍历，调用 `GetFirstSet()` 计算具体 FIRST 集。

```cpp
// Get all first sets:
void SyntaxParser::GetAllFirstSets()
{
  // For terminal symbols, the first set is itself:
  for (int i = 1; i < eps_idx_ + 1; ++i) first_sets_[i].insert(i);

  for (int i = 0; i < productions_.size(); ++i)
  {
    if (productions_[i][0] == productions_[i][1]) continue;
    GetFirstSet(productions_[i][0], i);
  }
}

// Get first set of a non-terminal symbol:
void SyntaxParser::GetFirstSet(const int& sym_id, const int& prod_idx)
{
  int right_first_id = productions_[prod_idx][1];
  if (right_first_id <= eps_idx_)  // First symbol is a terminal
    first_sets_[sym_id].insert(right_first_id);
  else  // First symbol is a non-terminal
  {
    for (int i = 0; i < idx_table_[right_first_id].size(); ++i)
    {
      if (productions_[prod_idx][0] == productions_[prod_idx][1]) continue;
      GetFirstSet(right_first_id, idx_table_[right_first_id][i]);
    }
    first_sets_[sym_id].insert(first_sets_[right_first_id].begin(),
                               first_sets_[right_first_id].end());
  }
}
```

### 5.4 构造 DFA

构造项目集族所使用函数为 `GetItemSetCollection()`，其中需要构造闭包，调用函数 `GetClosure()`。在使用有向边存储 DFA 图时，使用到函数 `AddEdge()`。

项目集族存储在 `item_set_collection_` 中，有向边存储在 `edge_set_` 中，每个项目集的头结点存储在 `head_` 中。

```cpp
// Get item set collection:
void SyntaxParser::GetItemSetCollection()
{
  vector<Item> temp;
  Item it;
  it.first_set.insert(0);
  temp = GetClosure(it);
  queue<vector<Item>> q;  // BFS
  q.push(temp);
  item_set_collection_.push_back(temp);
  while (!q.empty())
  {
    vector<Item> cur = q.front();
    q.pop();
    for (int i = 1; i <= sym_num_; i++)
    {
      if (i == eps_idx_) continue;
      temp.clear();
      for (int j = 0; j < cur.size(); j++)
      {
        if (cur[j].dot_pos ==
            productions_[cur[j].prod_idx].size())  // reduction
          continue;
        int dot_next = productions_[cur[j].prod_idx][cur[j].dot_pos];
        if (dot_next == i)
        {
          Item it;
          it.first_set = cur[j].first_set;
          it.dot_pos = cur[j].dot_pos + 1;
          it.prod_idx = cur[j].prod_idx;
          temp = CombineSet(temp, GetClosure(it));
        }
      }
      if (temp.size() == 0) continue;
      int cur_node = FindSetInCollection(cur, item_set_collection_);
      int target_node = FindSetInCollection(temp, item_set_collection_);
      if (target_node == -1)
      {
        item_set_collection_.push_back(temp);
        q.push(temp);
        AddEdge(cur_node, item_set_collection_.size() - 1, i);
      }
      else
        AddEdge(cur_node, target_node, i);
    }
  }
}

// Get closure of an item, using BFS:
vector<Item> SyntaxParser::GetClosure(Item& it)
{
  vector<Item> closure;
  closure.push_back(it);
  queue<Item> stack;
  stack.push(it);
  while (!stack.empty())
  {
    Item top_it = stack.front();
    stack.pop();
    if (top_it.dot_pos ==
        productions_[top_it.prod_idx].size())  // . is at the end
      continue;
    int sym_id_1 =
        productions_[top_it.prod_idx][top_it.dot_pos];  // symbol after .
    if (sym_id_1 <= eps_idx_)                           // terminal
      continue;
    for (int i = 0; i < idx_table_[sym_id_1].size(); i++)
    {
      Item temp_it;
      temp_it.prod_idx = idx_table_[sym_id_1][i];
      if (productions_[top_it.prod_idx].size() - top_it.dot_pos == 1)
        temp_it.first_set += top_it.first_set;
      else
      {
        int sym_id_2 = productions_[top_it.prod_idx][top_it.dot_pos + 1];
        temp_it.first_set.insert(first_sets_[sym_id_2].begin(),
                                 first_sets_[sym_id_2].end());
      }
      if (!IsItemInSet(temp_it, closure))
      {
        stack.push(temp_it);
        closure.push_back(temp_it);
      }
    }
  }
  return closure;
}

// construct an edge:
void SyntaxParser::AddEdge(const int& from, const int& to, const int& w)
{
  int f = head_.count(from) == 0 ? -1 : head_[from];
  Edge e = {f, to, w};
  edge_set_.push_back(e);
  head_[from] = edge_set_.size() - 1;
}
```

### 5.5 生成预测分析表

生成预测分析表所使用函数为 `GetParsingTable()`，其中需要判断是否存在冲突。预测分析表存储在 `parsing_table_` 中，移进/规约状态存储在 `sft_rdc_` 中。

```cpp
// Construct the parsing table:
// 0: conflict; -1: shift; -2: reduce; -3: accept
bool SyntaxParser::GetParsingTable()
{
  // Initialize:
  int size = item_set_collection_.size() + 1;
  parsing_table_ = vector<vector<int>>(size, vector<int>(size, -1));
  sft_rdc_ = vector<vector<int>>(size, vector<int>(size, -1));

  for (int i = 0; i < item_set_collection_.size(); i++)
  {
    if (head_.count(i) == 0) continue;
    for (int j = head_[i]; j != -1; j = edge_set_[j].next)
    {
      if (parsing_table_[i][edge_set_[j].w] != -1)  // conflict
        return -1;
      parsing_table_[i][edge_set_[j].w] = edge_set_[j].to;
      sft_rdc_[i][edge_set_[j].w] = -1;  // shift
    }
  }

  for (int i = 0; i < item_set_collection_.size(); i++)
    for (int j = 0; j < item_set_collection_[i].size(); j++)
      if (item_set_collection_[i][j].dot_pos ==
          productions_[item_set_collection_[i][j].prod_idx]
              .size())  // reduction
        for (auto& k : item_set_collection_[i][j].first_set)
        {
          if (parsing_table_[i][k] != -1)  // conflict
            return -1;
          if (k == 0 && item_set_collection_[i][j].prod_idx == 0)  // accept
            parsing_table_[i][k] = -3;
          else
          {
            parsing_table_[i][k] = item_set_collection_[i][j].prod_idx;
            sft_rdc_[i][k] = -2;  // reduction
          }
        }

  return 0;
}
```

一些打印函数不再展示。

## 6. 测试用例

测试文件在 `./Test` 目录下。

其中 `test_1.txt` 为所要读入的 CFG 文件，内容如下：

```txt
A -> B
B -> B + C | C
C -> C * D | D
D -> ( B ) | x
```

另一文件 `终结符&非终结符.txt` 给出了以上 CFG 中的终结符和非终结符，需要复制粘贴在程序中，内容如下：

```txt
非终结符：A B C D
终结符：x + * ( )
```

之后输入待分析的串，这里使用的例子是 `x * ( x + x )`，输入后，程序会输出分析过程。

程序运行结果如下：

```txt
Please enter terminal symbols(use spaces to separate): x + * ( )
Please enter non-terminal symbols(use spaces to separate): A B C D
Please enter the file path(eg:".\Test\test_1.txt"): .\Test\test_1.txt
Please enter a string to parse: x * ( x + x )


Terminal symbols: x + * ( ) ε
Non-terminal symbols: A B C D
Context-free grammars:
----------------------------------------
A -> B
B -> B + C | C
C -> C * D | D
D -> ( B ) | x
----------------------------------------


Symbol table:
 0  |  #
 1  |  x
 2  |  +
 3  |  *
 4  |  (
 5  |  )
 6  |  ε
 7  |  A
 8  |  B
 9  |  C
10  |  D

Productions:
 0  |  A -> B
 1  |  B -> B + C
 2  |  B -> C
 3  |  C -> C * D
 4  |  C -> D
 5  |  D -> ( B )
 6  |  D -> x

First sets:
FIRST(A) = ( x
FIRST(B) = ( x
FIRST(C) = ( x
FIRST(D) = ( x

Item sets:
Item set 0:
A B  |  1  |  #
B B + C  |  1  |  #
B C  |  1  |  #
B B + C  |  1  |  +
B C  |  1  |  +
C C * D  |  1  |  #
C D  |  1  |  #
C C * D  |  1  |  +
C D  |  1  |  +
C C * D  |  1  |  *
C D  |  1  |  *
D ( B )  |  1  |  #
D x  |  1  |  #
D ( B )  |  1  |  +
D x  |  1  |  +
D ( B )  |  1  |  *
D x  |  1  |  *

Item set 1:
D x  |  2  |  #
D x  |  2  |  +
D x  |  2  |  *

Item set 2:
D ( B )  |  2  |  #
B B + C  |  1  |  )
B C  |  1  |  )
B B + C  |  1  |  +
B C  |  1  |  +
C C * D  |  1  |  )
C D  |  1  |  )
C C * D  |  1  |  +
C D  |  1  |  +
C C * D  |  1  |  *
C D  |  1  |  *
D ( B )  |  1  |  )
D x  |  1  |  )
D ( B )  |  1  |  +
D x  |  1  |  +
D ( B )  |  1  |  *
D x  |  1  |  *
D ( B )  |  2  |  +
D ( B )  |  2  |  *

Item set 3:
A B  |  2  |  #
B B + C  |  2  |  #
B B + C  |  2  |  +

Item set 4:
B C  |  2  |  #
B C  |  2  |  +
C C * D  |  2  |  #
C C * D  |  2  |  +
C C * D  |  2  |  *

Item set 5:
C D  |  2  |  #
C D  |  2  |  +
C D  |  2  |  *

Item set 6:
D x  |  2  |  )
D x  |  2  |  +
D x  |  2  |  *

Item set 7:
D ( B )  |  2  |  )
B B + C  |  1  |  )
B C  |  1  |  )
B B + C  |  1  |  +
B C  |  1  |  +
C C * D  |  1  |  )
C D  |  1  |  )
C C * D  |  1  |  +
C D  |  1  |  +
C C * D  |  1  |  *
C D  |  1  |  *
D ( B )  |  1  |  )
D x  |  1  |  )
D ( B )  |  1  |  +
D x  |  1  |  +
D ( B )  |  1  |  *
D x  |  1  |  *
D ( B )  |  2  |  +
D ( B )  |  2  |  *

Item set 8:
D ( B )  |  3  |  #
B B + C  |  2  |  )
B B + C  |  2  |  +
D ( B )  |  3  |  +
D ( B )  |  3  |  *

Item set 9:
B C  |  2  |  )
B C  |  2  |  +
C C * D  |  2  |  )
C C * D  |  2  |  +
C C * D  |  2  |  *

Item set 10:
C D  |  2  |  )
C D  |  2  |  +
C D  |  2  |  *

Item set 11:
B B + C  |  3  |  #
C C * D  |  1  |  #
C D  |  1  |  #
C C * D  |  1  |  *
C D  |  1  |  *
D ( B )  |  1  |  #
D x  |  1  |  #
D ( B )  |  1  |  *
D x  |  1  |  *
B B + C  |  3  |  +
C C * D  |  1  |  +
C D  |  1  |  +
D ( B )  |  1  |  +
D x  |  1  |  +

Item set 12:
C C * D  |  3  |  #
D ( B )  |  1  |  #
D x  |  1  |  #
C C * D  |  3  |  +
D ( B )  |  1  |  +
D x  |  1  |  +
C C * D  |  3  |  *
D ( B )  |  1  |  *
D x  |  1  |  *

Item set 13:
D ( B )  |  3  |  )
B B + C  |  2  |  )
B B + C  |  2  |  +
D ( B )  |  3  |  +
D ( B )  |  3  |  *

Item set 14:
B B + C  |  3  |  )
C C * D  |  1  |  )
C D  |  1  |  )
C C * D  |  1  |  *
C D  |  1  |  *
D ( B )  |  1  |  )
D x  |  1  |  )
D ( B )  |  1  |  *
D x  |  1  |  *
B B + C  |  3  |  +
C C * D  |  1  |  +
C D  |  1  |  +
D ( B )  |  1  |  +
D x  |  1  |  +

Item set 15:
D ( B )  |  4  |  #
D ( B )  |  4  |  +
D ( B )  |  4  |  *

Item set 16:
C C * D  |  3  |  )
D ( B )  |  1  |  )
D x  |  1  |  )
C C * D  |  3  |  +
D ( B )  |  1  |  +
D x  |  1  |  +
C C * D  |  3  |  *
D ( B )  |  1  |  *
D x  |  1  |  *

Item set 17:
B B + C  |  4  |  #
C C * D  |  2  |  #
C C * D  |  2  |  *
B B + C  |  4  |  +
C C * D  |  2  |  +

Item set 18:
C C * D  |  4  |  #
C C * D  |  4  |  +
C C * D  |  4  |  *

Item set 19:
D ( B )  |  4  |  )
D ( B )  |  4  |  +
D ( B )  |  4  |  *

Item set 20:
B B + C  |  4  |  )
C C * D  |  2  |  )
C C * D  |  2  |  *
B B + C  |  4  |  +
C C * D  |  2  |  +

Item set 21:
C C * D  |  4  |  )
C C * D  |  4  |  +
C C * D  |  4  |  *


    D
 0 ---> 5
    C
 0 ---> 4
    B
 0 ---> 3
    (
 0 ---> 2
    x
 0 ---> 1
    D
 2 ---> 10
    C
 2 ---> 9
    B
 2 ---> 8
    (
 2 ---> 7
    x
 2 ---> 6
    +
 3 ---> 11
    *
 4 ---> 12
    D
 7 ---> 10
    C
 7 ---> 9
    B
 7 ---> 13
    (
 7 ---> 7
    x
 7 ---> 6
    )
 8 ---> 15
    +
 8 ---> 14
    *
 9 ---> 16
    D
11 ---> 5
    C
11 ---> 17
    (
11 ---> 2
    x
11 ---> 1
    D
12 ---> 18
    (
12 ---> 2
    x
12 ---> 1
    )
13 ---> 19
    +
13 ---> 14
    D
14 ---> 10
    C
14 ---> 20
    (
14 ---> 7
    x
14 ---> 6
    D
16 ---> 21
    (
16 ---> 7
    x
16 ---> 6
    *
17 ---> 12
    *
20 ---> 16

Parsing table:
     |  #  |  x  |  +  |  *  |  (  |  )  |  A  |  B  |  C  |  D  |
0    |     | s1  |     |     | s2  |     |     | s3  | s4  | s5  |
1    | r6  |     | r6  | r6  |     |     |     |     |     |     |
2    |     | s6  |     |     | s7  |     |     | s8  | s9  | s10 |
3    | acc |     | s11 |     |     |     |     |     |     |     |
4    | r2  |     | r2  | s12 |     |     |     |     |     |     |
5    | r4  |     | r4  | r4  |     |     |     |     |     |     |
6    |     |     | r6  | r6  |     | r6  |     |     |     |     |
7    |     | s6  |     |     | s7  |     |     | s13 | s9  | s10 |
8    |     |     | s14 |     |     | s15 |     |     |     |     |
9    |     |     | r2  | s16 |     | r2  |     |     |     |     |
10   |     |     | r4  | r4  |     | r4  |     |     |     |     |
11   |     | s1  |     |     | s2  |     |     |     | s17 | s5  |
12   |     | s1  |     |     | s2  |     |     |     |     | s18 |
13   |     |     | s14 |     |     | s19 |     |     |     |     |
14   |     | s6  |     |     | s7  |     |     |     | s20 | s10 |
15   | r5  |     | r5  | r5  |     |     |     |     |     |     |
16   |     | s6  |     |     | s7  |     |     |     |     | s21 |
17   | r1  |     | r1  | s12 |     |     |     |     |     |     |
18   | r3  |     | r3  | r3  |     |     |     |     |     |     |
19   |     |     | r5  | r5  |     | r5  |     |     |     |     |
20   |     |     | r1  | s16 |     | r1  |     |     |     |     |
21   |     |     | r3  | r3  |     | r3  |     |     |     |     |

Parsing of the input string: x * ( x + x )
step   state_stack          symbol_stack          input      action
 0     0                    #                     x*(x+x)#   action[0,1] = 1, push 1
 1     0  1                 #  x                  *(x+x)#    D->x: reduce, goto[0, 3] = 5, push 5
 2     0  5                 #  D                  *(x+x)#    C->D: reduce, goto[0, 3] = 4, push 4
 3     0  4                 #  C                  *(x+x)#    action[4,3] = 12, push 12
 4     0  4  12             #  C  *               (x+x)#     action[12,4] = 2, push 2
 5     0  4  12 2           #  C  *  (            x+x)#      action[2,1] = 6, push 6
 6     0  4  12 2  6        #  C  *  (  x         +x)#       D->x: reduce, goto[2, 2] = 10, push 10
 7     0  4  12 2  10       #  C  *  (  D         +x)#       C->D: reduce, goto[2, 2] = 9, push 9
 8     0  4  12 2  9        #  C  *  (  C         +x)#       B->C: reduce, goto[2, 2] = 8, push 8
 9     0  4  12 2  8        #  C  *  (  B         +x)#       action[8,2] = 14, push 14
 10    0  4  12 2  8  14    #  C  *  (  B  +      x)#        action[14,1] = 6, push 6
 11    0  4  12 2  8  14 6  #  C  *  (  B  +  x   )#         D->x: reduce, goto[14, 5] = 10, push 10
 12    0  4  12 2  8  14 10 #  C  *  (  B  +  D   )#         C->D: reduce, goto[14, 5] = 20, push 20
 13    0  4  12 2  8  14 20 #  C  *  (  B  +  C   )#         B->B+C: reduce, goto[2, 5] = 8, push 8
 14    0  4  12 2  8        #  C  *  (  B         )#         action[8,5] = 15, push 15
 15    0  4  12 2  8  15    #  C  *  (  B  )      #          D->(B): reduce, goto[12, 0] = 18, push 18
 16    0  4  12 18          #  C  *  D            #          C->C*D: reduce, goto[0, 0] = 4, push 4
 17    0  4                 #  C                  #          B->C: reduce, goto[0, 0] = 3, push 3
 18    0  3                 #  B                  #          Accept!
```

经检查，结果正确。

接下来，换一个文法不正确的输入，如 `( x * x x` ，其括号未闭合且出现了两个连续的 `x` ，此时应该报错，运行结果如下：

```txt
Please enter terminal symbols(use spaces to separate): x + * ( )
Please enter non-terminal symbols(use spaces to separate): A B C D
Please enter the file path(eg:".\Test\test_1.txt"): .\Test\test_1.txt
Please enter a string to parse: ( x * x x

/********************************************************
 由于 CFG 没有变化所以构建的分析表与上文相同，省略以节省篇幅。
*********************************************************/

Parsing of the input string: ( x * x x
step   state_stack       symbol_stack      input      action
 0     0                 #                 (x*xx#     action[0,4] = 2, push 2
 1     0  2              #  (              x*xx#      action[2,1] = 6, push 6
 2     0  2  6           #  (  x           *xx#       D->x: reduce, goto[2, 3] = 10, push 10
 3     0  2  10          #  (  D           *xx#       C->D: reduce, goto[2, 3] = 9, push 9
 4     0  2  9           #  (  C           *xx#       action[9,3] = 16, push 16
 5     0  2  9  16       #  (  C  *        xx#        action[16,1] = 6, push 6
Error!
```