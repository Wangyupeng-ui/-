#include "lex.h"

lex::lex(){}

lex::~lex(){}

bool lex::IsDigit(const char& ch)
{
	return (ch >= '0' && ch <= '9');
}

bool lex::IsLetter(const char& ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

int lex::IsKeyword(const string& wd)
{
	if (wd == "int" || wd == "main" || wd == "char" || wd == "for" || wd == "if" || wd == "else" || wd =="return") {
		this->result_ += "<±£Áô×Ö ";
		this->result_ += wd;
		this->result_ += " 0>\n";
		return 1;
	}
	else {
		return 0;
	}
}

int lex::ReadFile(const string& file_path)
{
	ifstream file;

	file.open(file_path, ios::in);
	if (!file.is_open())
	{
		cout << "\nFailed to open the file!" << endl;
		return -1;
	}
	while (!file.eof())
	{
		string line;
		getline(file, line);
		this->content_ += line + "\n";
	}
	file.close();
	return 0;
}

void lex::PrintContent()
{
	cout << "\nContent of the file:\n\n"
		<< "----------------------------------------\n\n"
		<< this->content_ << "----------------------------------------\n\n";
}

void lex::Analysis()
{
	int index = 0;
	char ch = this->content_[0];
	while (this->content_[index])
	{
		if (IsLetter(ch)) {
			string temp = "";
			while (IsDigit(ch)|| IsLetter(ch)) {
				temp += ch;
				ch = this->content_[++index];
			}
			if (!IsKeyword(temp)) {
				this->result_ += "<±êÊ¶·û ";
				this->result_ += temp;
			    this->result_ += " 1>\n";
			}
		}
		if (ch == '[') {
			this->result_ += "<²Ù×÷·û [ 2>\n";
			ch = this->content_[++index];
		}
		if (ch == ']') {
			this->result_ += "<²Ù×÷·û ] 3 >\n";
			ch = this->content_[++index];
		}
		if (ch == ';') {
			this->result_ += "<²Ù×÷·û ; 4 >\n";
			ch = this->content_[++index];
		}
		if (ch == '=') {
			this->result_ += "<²Ù×÷·û = 5>\n";
			ch = this->content_[++index];
		}
		if (ch == '<') {
			this->result_ += "<²Ù×÷·û < 6>\n";
			ch = this->content_[++index];
		}
		if (ch == '++') {
			this->result_ += "<²Ù×÷·û ++ 7>\n";
			ch = this->content_[++index];
		}
		if (ch == '(') {
			this->result_ += "<²Ù×÷·û ( 8>\n";
			ch = this->content_[++index];
		}
		if (ch == ')') {
			this->result_ += "<²Ù×÷·û ) 9>\n";
			ch = this->content_[++index];
		}
		if (ch == '{') {
			this->result_ += "<²Ù×÷·û { 10>\n";
			ch = this->content_[++index];
		}
		if (ch == '}') {
			this->result_ += "<²Ù×÷·û } 11>\n";
			ch = this->content_[++index];
		}
		if (ch == '\'') {
			ch = this->content_[++index];
			if (IsLetter(ch)) {
				this->result_ += "<×Ö·û ";
				this->result_ += ch;
				this->result_ += " 14>\n";
			}
			ch = this->content_[++index];
			ch = this->content_[++index];
		}
		if (IsDigit(ch)) {
			this->result_ += "<ÕûÊý ";
			while (IsDigit(ch)) {
				this->result_ += ch;
				ch = this->content_[++index];
			}
			this->result_ += " 15>\n";
		}
		else {
			ch = this->content_[++index];
		}
	}
}

void lex::WriteResult(const string& file_path)
{
	ofstream file;
	string result_file_path =
		file_path.substr(0, file_path.length() - 4) + "_result.txt";
	file.open(result_file_path, ios::out);
	if (!file.is_open())
	{
		cout << "\nFailed to open the file!" << endl;
	}
	file << this->result_;
	file.close();
	cout << "\nResult has been written to \"" << result_file_path << "\"\n";
}


