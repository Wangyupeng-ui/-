
#include "lex.h"

int main()
{
    lex lex;
    string file_path;

    /***** �����ļ� *****/
    while (true)
    {
        cout << "Please enter the file path: ";
        getline(cin, file_path);
        if (!lex.ReadFile(file_path)) break;
    }

    /***** ��ӡ�ļ� *****/
    lex.PrintContent();

    /***** �ʷ����� *****/
    lex.Analysis();

    /***** д������ *****/
    lex.WriteResult(file_path);

    system("pause");
    return 0;
}