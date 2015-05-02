#ifndef DELETE_H_
#define DELETE_H_

#include "database.h"
#include "token.h"

void matchName(std::string target);
void matchType(std::string target);
bool checkName(std::string target);
bool checkType(std::string target);
void nextToken();


std::vector<int> where_clause();
std::vector<bool> disjunct();
std::vector<bool> conjunct();
std::vector<bool> _bool();
std::vector<bool> comp();
std::vector<double> expr();
std::vector<double> term();
std::vector<double> unary();

std::vector<bool> genVector(bool value);
std::vector<double> genVector(double value);
std::vector<double> genVector(std::string columnName);

// accumulative logical operation
void compare(std::vector<bool> &result, std::string op, std::vector<bool> value);
// relation operation
std::vector<bool> compare(std::vector<double> lhs, std::string op, std::vector<double> rhs);
// accumulative arithmetic operation
void operation(std::vector<double> &result, std::string op, std::vector<double> value);

void deleteParse(Database& db, std::vector<Token> _tokens);

std::string TransLow(std::string str);


void deleteOperation(Database&, std::vector<int>);

#endif
