#ifndef TABLE_H_
#define TABLE_H_

#include "column.h"

class Table {
	public:
		Table(std::string name, std::vector<Column> columns, 
				std::vector<std::string> primaryKey);
		std::string getName();
		std::vector<Column> getColumns();
		Column getColumn(std::string name);
		Column& getColumnReference(std::string name);
		int getNumberOfRows();
		std::vector<std::string> getPrimaryKey();
		bool isColumnExist(std::string name);
		void deleteRows(std::vector<int>);
		void deleteAllRows();
		
	private:
		std::string name;
		std::vector<std::string> primaryKey;
		std::vector<Column> columns;
};

#endif
