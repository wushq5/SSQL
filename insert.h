#ifndef INSERT_H
#define INSERT_H

#include "token.h"
#include "database.h"

extern std::string TransLow(std::string str);

extern bool checkType(std::string type);
extern bool checkName(std::string name);
extern void matchType(std::string type);
extern void matchName(std::string name);
extern void nextToken();

int simple_expr();
int simple_term();
int simple_unary();

// accumulative arithmetic operation
void operation(int &result, std::string op, int value = 0);


void insert(std::vector<Token> tokens, Database &db);

#endif
