#ifndef QUERY_H_
#define QUERY_H_

#include "database.h"
#include "token.h"

extern void matchName(std::string target);
extern void matchType(std::string target);
void matchSelected(std::string target);
extern bool checkName(std::string target);
extern bool checkType(std::string target);
extern void nextToken();

/*
void Q_bool();
void Qconjunct_list2();
void Qconjunct_list();
void Qwhere_clause();
*/
void select_list();
void Qcolumn_list();
void Qcolumn_list2();
extern std::vector<int> where_clause();

std::vector<Column> queryRows(Table table, std::vector<int> index);
void queryParse(Database& db, std::vector<Token> _tokens);
extern std::string TransLow(std::string str);

// void queryOperation(Database&, std::vector<std::string> id, std::vector<std::string> selected,
//	std::vector<int> number, std::vector<std::string> rop);
// bool QropIsTrue(std::string rop, int, int);
void queryOperation(Database&, std::vector<std::string> selected,
	std::vector<int> rowIndex);

#endif