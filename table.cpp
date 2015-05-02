#include <iostream>
#include "table.h"

Table::Table(std::string name, std::vector<Column> columns, 
        std::vector<std::string> primaryKey) {
	this->name = name;
	this->columns = columns;
	this->primaryKey = primaryKey;
}

std::string Table::getName() {
	return name;
}

std::vector<Column> Table::getColumns() {
    return columns;
}

std::vector<std::string> Table::getPrimaryKey() {
	return primaryKey;
}

/*
 * get column by name
 */
Column Table::getColumn(std::string name) {
	std::vector<Column>::iterator iter;
	for (iter = columns.begin(); iter != columns.end(); iter++) {
		if ((*iter).getName() == name) {
  			return *iter;
  			break;
  		}
	}
	// if don't exist, return null
	std::vector<int> nul;
	std::string fuck = "";
	Column col(fuck, nul, false, -1);
	return col;
}

Column& Table::getColumnReference(std::string name) {
  std::vector<Column>::iterator iter;
  for (iter = columns.begin(); iter != columns.end(); iter++) {
    if ((*iter).getName() == name) {
        return *iter;
        break;
      }
  }
  // if don't exist, return null
  static std::vector<int> nul;
  static std::string fuck = "";
  static Column col(fuck, nul, false, -1);
  return col;
}

int Table::getNumberOfRows() {
  if (columns.size() == 0) return 0;
  else return columns[0].getElements().size();
}

/*
 * check whether the column is exist
 */
bool Table::isColumnExist(std::string name) {
    bool isExist = false;

    std::vector<Column>::iterator iter;
	for (iter = columns.begin(); iter != columns.end(); iter++) {
		if ((*iter).getName().compare(name) == 0) {
  			isExist = true;
  			break;
  		}
	}
	
	return isExist;
}

void Table::deleteRows(std::vector<int> rowIndex) {

    std::vector<Column>::iterator i;
    for (i = columns.begin(); i != columns.end(); i++) {
        
        std::vector<int>::iterator it;
        for (it = rowIndex.begin(); it != rowIndex.end(); it++) {
            (*i).deleteV(*it);
        }
    }
}

void Table::deleteAllRows() {
    std::vector<Column>::iterator i;
    for (i = columns.begin(); i != columns.end(); i++) {
        (*i).deleteAll();
    }
}

