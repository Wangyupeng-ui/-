
#include "lex.h"

int main()
{
    lex lex;
    string file_path;

    /***** 读入文件 *****/
    while (true)
    {
        cout << "Please enter the file path: ";
        getline(cin, file_path);
        if (!lex.ReadFile(file_path)) break;
    }

    /***** 打印文件 *****/
    lex.PrintContent();

    /***** 词法分析 *****/
    lex.Analysis();

    /***** 写出序列 *****/
    lex.WriteResult(file_path);

    system("pause");
    return 0;
}