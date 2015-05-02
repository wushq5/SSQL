#ifndef CREATE_H_
#define CREATE_H_

#include <iostream>
#include <sstream>
#include <stack>
#include <algorithm>
#include <string>
#include <stack>
#include "lexer.h"
#include "token.h"
#include "database.h"

         
class Create {
	public:
		Create(std::vector<Token> tokens);
		Table create();
		void printTest(bool);
};

bool create_stmt();
void decl_list();
void decl_list2();
void decl();
void default_spec();
void Csimple_expr();
void Csimple_expr2();
void Csimple_term();
void Csimple_term2();
void Csimple_unary();
void column_list();
void column_list2();

void match(std::string type, std::string name);
Token getNextToken();
std::stack<std::string> RPN(std::vector<std::string>);
int calculateRPN(std::stack<std::string>);
std::string intToStr(int);
bool detectError(Table);


#endif
