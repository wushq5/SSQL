#include "database.h"

Database::Database(std::string name, std::vector<Table> db) {
    this->name = name;
	this->db = db;
}

std::vector<Table> Database::getTables() {
    return db;
}

Table Database::getTable(std::string name) {
    std::vector<Table>::iterator iter;
	for (iter = db.begin(); iter != db.end(); iter++) {
		if ((*iter).getName().compare(name) == 0) {
  			return *iter;
  		}
	}
	// if don't exist, return null
    std::vector<Column> nul;
    std::vector<std::string> pk;
    std::string fuck = "";
    Table t(fuck, nul, pk);
    return t;
}

Table& Database::getTableRef(std::string name) {
    std::vector<Table>::iterator iter;
	for (iter = db.begin(); iter != db.end(); iter++) {
		if ((*iter).getName().compare(name) == 0) {
  			return *iter;
  		}
	}
	// if don't exist, return null
    static std::vector<Column> nul;
    static std::vector<std::string> pk;
    static std::string fuck = "";
    static Table t(fuck, nul, pk);
    return t;
}

void Database::addTable(Table t) {
	db.push_back(t);
}



/*
 * check whether the table is exist
 */
bool Database::isTableExist(std::string name) {
    bool isExist = false;

    std::vector<Table>::iterator iter;
	for (iter = db.begin(); iter != db.end(); iter++) {
		if ((*iter).getName().compare(name) == 0) {
  			isExist = true;
  			break;
  		}
	}
	
	return isExist;
}
