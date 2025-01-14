#pragma once

#include <fstream>
#include <iostream>
#include <string>

using namespace std;
class lex
{
public:
	lex();
	~lex();

	int ReadFile(const string& file_path);
	void PrintContent();
	void Analysis();
	void WriteResult(const string& file_path);
private:
	bool IsDigit(const char& ch);
	bool IsLetter(const char& ch);
	int IsKeyword(const string& wd);

	string content_;
	string result_;
};

