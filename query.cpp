#include <iostream>
#include <iomanip>
#include "query.h"

extern Token lookahead;
extern std::vector<Token> tokens;
extern int tokenIndex;
extern bool hasError;
extern Table *currentTable;
std::vector<std::string> selected;
std::vector<Column> result;
bool all = false;


void queryParse(Database& db, std::vector<Token> _tokens) {
	selected.clear();
	tokenIndex = 0; 
	hasError = false;
	lookahead = _tokens.front();
	tokens = _tokens;

	// predictive
	matchName("select"); 
	select_list();
	matchName("from"); 

	// check the type and judge whether the table is in the db
	if (checkType("id")) {
		std::string tableName = lookahead.getName();
		if (db.isTableExist(tableName)) {
			currentTable = &db.getTableRef(tableName);
			nextToken();
		} else {
			hasError = true;
			std::cerr << "Table doesn't exist." << std::endl;
			return;
		}
	} else {
		std::cerr << "ID error." << std::endl;
		hasError = true;
	}
	//get selected row index after where condition
	std::vector<int> rowIndex = where_clause();
	if (hasError) {
		std::cerr << "Expression error." << std::endl;
		return;
	}

	matchName(";");

	if (!hasError) {
		queryOperation(db, selected, rowIndex);
	} else {
		std:: cout << "Error!" << std::endl;
		 hasError  =  true;
	}
	
}

//return all column if select_list equals "*",else jump to  Qcolumn_list funtion
void select_list() {
	if (!hasError && lookahead.getName() == "*") {
		all = true;
		matchName("*");
	}
	else if (!hasError && lookahead.getType() == "id") {
		Qcolumn_list();
		all = false;
	}
}

//get selected column from  given id name
void Qcolumn_list() {
	if (!hasError && lookahead.getType() == "id") {
		matchSelected("id");
		Qcolumn_list2();
	}
}

void Qcolumn_list2() {
	if (!hasError && lookahead.getName() == ",") {
		matchName(","); 
		matchSelected("id");
		Qcolumn_list2();
	}
}

//save the selected id into vector<string> selected
void matchSelected(std::string target) {
	std::string str = lookahead.getType();
	if (str.compare(target) == 0) {
		if (tokenIndex + 1 < tokens.size()) {
			lookahead = tokens[tokenIndex + 1];
			// put Qid into vector
			if (target.compare("id") == 0)
				selected.push_back(tokens[tokenIndex].getName());
			tokenIndex++;
		}

	}
	else
		hasError = true;
}

std::string QTransLow(std::string str) {
	for (int i = 0; i < str.size(); i++) {
			if (str[i] >= 'A' && str[i] <= 'Z')
				str[i] = str[i] + 32;
	}
	return str;
}

//query and print
void queryOperation(Database& db, std::vector<std::string> selected,
			std::vector<int> rowIndex) {
	std::vector<int> &queryIndex = rowIndex;

	// check whether table exist
	if (currentTable != NULL) {
		//get all columns from table
		std::vector<Column> columns = currentTable->getColumns();
		//store the selected columns in result
		result = queryRows(*currentTable, queryIndex);
		if(!hasError) {
			//output format
			for (int iter = 0; iter < result.size(); iter++) {
				std::cout << "|------------";
			}
			std::cout << "|" << std::endl;
			for (std::vector<Column>::iterator iter = result.begin(); iter != result.end(); iter++) {
				std::cout << "|";
				for(int i = 0; i < (12 - (*iter).getName().size()) / 2; i++) {
					std::cout << " ";
				}
				std::cout << (*iter).getName();
				for(int i = 0; i < (12 - (*iter).getName().size() + 1) / 2; i++) {
					std::cout << " ";
				}
			}

			std::cout << "|" << std::endl;
			for (std::vector<Column>::iterator iter = result.begin(); iter != result.end(); iter++) {
				std::cout << "|------------";
			}
			std::cout << "|" << std::endl;

			int num = 0;
			//tranverse all columns and compare it to the selected columns, once matched, print the element according the selected rows
			for (std::vector<int>::iterator iter1 = queryIndex.begin(); iter1 != queryIndex.end(); iter1++, num++) {	
				for (std::vector<Column>::iterator iter = result.begin(); iter != result.end(); iter++) {		
					for (std::vector<Column>::iterator iter2 = columns.begin(); iter2 != columns.end(); iter2++) {
						//judge if the selected colum's name equal the table' column's name
						if ((*iter).getName() == (*iter2).getName()) {
							std::cout << "|";
							int l = 0;
							int length = (*iter2).getValue(*iter1);
							if (length <= 0) {
								l = 1;
								length = -length;
							}
							while(length > 0) {
								length = length /10;
								l++;
							}
							for(int i = 0; i < (12 - l) / 2; i++) {
								std::cout << " ";
							}
							std::cout << (*iter2).getValue(*iter1);
							for(int i = 0; i < (12 - l + 1) / 2; i++) {
								std::cout << " ";
							}
						}	
					}
				}
				std::cout << "|" << std::endl;
				for (int i = 0; i < result.size(); i++) {
					std::cout << "|------------";
				}
				std::cout << "|" << std::endl;
			}
			std::cout << num << " row affected." << std::endl;
			std::cout << std::endl;
		} 

	}
}

//get rows from selected index and selected column
std::vector<Column> queryRows(Table table, std::vector<int> index) {
	std::vector<Column> tmpCol;
	std::vector<Column> allCol = table.getColumns();
	std::vector<std::string> tmp_select;
	//if the select_list equals "*", then select all columns
	if (all)  {
		for (std::vector<Column>::iterator iter = allCol.begin(); iter != allCol.end(); iter++)  {
			tmp_select.push_back((*iter).getName());
		}
	}
	else {
		tmp_select = selected;
	}
	//push back the selected rows into vector<Column>
	for (std::vector<std::string>::iterator coliter = tmp_select.begin(); coliter != tmp_select.end(); coliter++) {
		std::vector<int> newEle;
		for (std::vector<int>::iterator iter = index.begin(); iter != index.end(); iter++) {
			if (!hasError && table.isColumnExist(*coliter)) {
				//according  to selected rows and selected column,get the value  and save the value in newEle
				newEle.push_back(table.getColumn(*coliter).getValue(*iter));
			}
			else {
				std::cout << "Column name error: " << *coliter << " !\n";
				hasError = true;
				break;
			}
		}
		//construct a new column to save selected vlaue
		Column newcol(*coliter, newEle, false, -1);
		//save all selected column
		tmpCol.push_back(newcol);
	}
	return tmpCol;
}


