#ifndef DATABASE_H_
#define DATABASE_H_

#include <iostream>
#include <vector> 

#include "table.h"


class Database {
	public:
		Database(std::string name, std::vector<Table> db);
		std::vector<Table> getTables();
		Table getTable(std::string name);
		Table& getTableRef(std::string name);   // get reference to modify it
		void addTable(Table t);
		bool isTableExist(std::string name);
		
	private:
		std::vector<Table> db;
		std::string name;
};

#endif
