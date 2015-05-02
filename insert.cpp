#include <iostream>
#include "insert.h"

extern Token lookahead;
extern std::vector<Token> tokens;
extern int tokenIndex;
extern bool hasError;

//#define DEBUG

// 传入一条insert语句，计算插入条目
void insert(std::vector<Token> _tokens, Database &db) {

	tokens = _tokens;
	tokenIndex = 0;
	hasError = false;
	lookahead = tokens.front();

	// 匹配"INSERT INTO"
	matchName("insert");
	matchName("into");
	if (hasError) {
		std::cerr << "Keyword error." << std::endl;
		return;
	}

	// 匹配表名
	std::string tableName;
	if (checkType("id") == true) {
		tableName = lookahead.getName();
		nextToken();
	} else {
		hasError = true;
		std::cerr << "Table name should be a legal ID." << std::endl;
		return;
	}
	if (!db.isTableExist(tableName)) {
		hasError = true;
		std::cerr << "Table does not exist." << std::endl;
		return;
	}

	// 匹配左括号
	matchName("(");
	if (hasError) {
		std::cerr << "Lack of left parenthesis." << std::endl;
		return;
	}

	// 获取属性名称列表
	std::vector<std::string> attributes;
	bool stopFlag = false;
	bool attrFlag = true;
	bool rightParenFlag = true;
	for (; !stopFlag; nextToken()) {
		if (attrFlag == true && checkType("id")) {
			attributes.push_back(lookahead.getName());
			attrFlag = false;
			rightParenFlag = true;
		} else if (attrFlag == false && checkName(",")) {
			attrFlag = true;
			rightParenFlag = false;
		} else if (rightParenFlag == true && checkName(")")) {
			stopFlag = true;
		} else {
			hasError = true;
		}
		if (hasError) {
			std::cerr << "Attribute list error." << std::endl;
			return;
		}
	}

	// 匹配"VALUES"
	matchName("values");
	if (hasError) {
		std::cerr << "Keyword error." << std::endl;
		return;
	}

	// 匹配左括号
	matchName("(");
	if (hasError) {
		std::cerr << "Lack of left parenthesis." << std::endl;
		return;
	}

	// 获取值列表
	std::vector<int> values;
	stopFlag = false;
	bool valueFlag = true;
	rightParenFlag = true;
	for (; !stopFlag; ) {
		if (valueFlag == true) {
			values.push_back(simple_expr());
			valueFlag = false;
			rightParenFlag = true;
		} else if (valueFlag == false && checkName(",")) {
			valueFlag = true;
			rightParenFlag = false;
			nextToken();
		} else if (rightParenFlag == true && checkName(")")) {
			stopFlag = true;
			nextToken();
		} else {
			std::cout << "...0" << std::endl;
			hasError = true;
		}
		if (hasError) {
			std::cerr << "Value list error." << std::endl;
			return;
		}
	}

	// 匹配分号
	matchName(";");
	if (hasError) {
		std::cerr << "Lack a semicolon." << std::endl;
		return;
	}

	// 检查属性间是否冲突
	for (int i = 0; i < attributes.size() - 1; i++) {
		for (int j = i + 1; j < attributes.size(); j++) {
			if (attributes[i] == attributes[j]) {
				hasError = true;
				std::cerr << "Redundant attributes." << std::endl;
				return;
			}
		}
	}

	// 检查属性和值的个数是否匹配
	if (attributes.size() != values.size()) {
		hasError = true;
		std::cerr << "The number of attributes and values does not match." << std::endl;
		return;
	}

	Table &table = db.getTableRef(tableName);
	std::vector<Column> columns = table.getColumns();

	// 检查主关键字冲突
	std::vector<std::string> primaryKeys = table.getPrimaryKey();
	const bool hasPrimaryKey = (primaryKeys.size() != 0);
	std::vector<bool> primaryKeyExists(primaryKeys.size());
	std::vector<int> primaryKeyValues(primaryKeys.size());
	if (hasPrimaryKey == true) {
		for (int i = 0; i < primaryKeys.size(); i++) {
			for (int j = 0; j < attributes.size(); j++) {
				if (primaryKeys[i] == attributes[j]) {
					primaryKeyExists[i] = true;
					primaryKeyValues[i] = values[j];
				}
			}
			if (primaryKeyExists[i] == false && 
				table.getColumn(primaryKeys[i]).getHasDefault() == true) {
				primaryKeyExists[i] = true;
				primaryKeyValues[i] = table.getColumn(primaryKeys[i]).getDefaultValue();
			}
		}
		for (int i = 0; i < primaryKeyExists.size(); i++) {
			if (primaryKeyExists[i] == false) {
				hasError = true;
				std::cerr << "Primary key hasn't been specified." << std::endl;
				return;
			}
		}
	}
	
	std::vector<Column> primaryKeyColumns;
	int columnSize = table.getColumns()[0].getElements().size();
	for (int i = 0; i < primaryKeys.size(); i++) {
		primaryKeyColumns.push_back(table.getColumn(primaryKeys[i]));
	}
	


	if (hasPrimaryKey == true) {
		for (int i = 0; i < columnSize; i++) {
			bool primaryKeyConflict = true;
			for (int j = 0; j < primaryKeys.size(); j++) {
				if (primaryKeyValues[j] == primaryKeyColumns[j].getValue(i)) {
					primaryKeyConflict = primaryKeyConflict && true;
				} else {
					primaryKeyConflict = primaryKeyConflict && false;
				}
			}
			if (primaryKeyConflict == true) {
				hasError = true;
				std::cerr << "Primary key is already exist." << std::endl;
				return;
			}
		}
	}

	// 检查是否有不存在的属性
	for (int i = 0; i < columns.size(); i++) {
		std::string columnAttr = columns[i].getName();
		bool columnAttrExist = false;
		if (columns[i].getHasDefault() == true) {
			columnAttrExist = true;
		}
		for (int j = 0; j < attributes.size(); j++) {
			if (columns[i].getName() == attributes[j]) {
				columnAttrExist = true;
			}
		}
		if (columnAttrExist == false) {
			hasError = true;
			std::cerr << "Some attribute without default is missing or don't exist." << std::endl;
			return;
		}
	}
	for (int i = 0; i < attributes.size(); i++) {
		std::string attr = attributes[i];
		bool attrExist = false;
		for (int j = 0; j < columns.size(); j++) {
			if (attributes[i] == columns[j].getName()) {
				attrExist = true;
			}
		}
		if (attrExist == false) {
			hasError = true;
			std::cerr << "Some attribute is not exist." << std::endl;
			return;
		}
	}

	// 插入值
	for (int i = 0; i < columns.size(); i++) {
		bool needDefaultValue = true;
		Column &currentColumn =
			table.getColumnReference(columns[i].getName());
		for (int j = 0; j < attributes.size(); j++) {
			if (attributes[j] == columns[i].getName()) {
				needDefaultValue = false;
				currentColumn.insert(values[j]);
			}
		}
		if (needDefaultValue == true) {
			currentColumn.insert(currentColumn.getDefaultValue());
		}
	}
	

	std::cout << "Insert operation success!" << std::endl;
	return;

}





int simple_expr() {

	#ifdef DEBUG
	std::cout << "simple_expr()"  << std::endl;
	#endif

	int result = 0;		// 累加结果
	bool stopFlag = false;
	std::string op("+");

	for (; !stopFlag && !hasError;) {
		// 处理数字
		if (checkType("num") || checkName("+") || checkName("-") || checkName("(")) {
			operation(result, op, simple_term());
		} else {

			#ifdef DEBUG
			std::cout << "...1" << std::endl;
			#endif

			hasError = true;
		}

		// 若接下来有符号，则继续，否则停止
		if (checkName("+") || checkName("-")) {
			op = lookahead.getName();
			nextToken();
		} else if (checkName(",") || checkName(")")) {
			stopFlag = true;
		} else {

			#ifdef DEBUG
			std::cout << "...2" << std::endl;
			#endif

			hasError = true;
		}
	}

	#ifdef DEBUG
	std::cout << "expr result = " << result << std::endl;
	#endif

	return result;
}

int simple_term() {

	#ifdef DEBUG
	std::cout << "simple_term()"  << std::endl;
	#endif

	int result = 1;		// 累乘结果
	bool stopFlag = false;
	std::string op("*");
	
	for (; !stopFlag && !hasError;) {
		// 处理数字
		if (checkType("num") || checkName("+") || checkName("-") || checkName("(")) {
			operation(result, op, simple_unary());
		} else {

			#ifdef DEBUG
			std::cout << "...3" << std::endl;
			#endif

			hasError = true;
		}

		// 若接下来有符号，则继续，否则停止
		if (checkName("*") || checkName("/")) {
			op = lookahead.getName();
			nextToken();
		} else if (checkName("+") || checkName("-") || checkName(",") || checkName(")")) {
			stopFlag = true;
		} else {

			#ifdef DEBUG
			std::cout << "...4" << std::endl;
			#endif

			hasError = true;
		}
	}

	#ifdef DEBUG
	std::cout << "term result = " << result << std::endl;
	#endif

	return result;
}

int simple_unary() {

	#ifdef DEBUG
	std::cout << "simple_unary()" << std::endl;
	#endif

	int result = 0;
	if (checkType("num")) {
		result = lookahead.getNum();
		nextToken();
	} else if (checkName("+")) {
		nextToken();
		result += simple_unary();
	} else if (checkName("-")) {
		nextToken();
		result -= simple_unary();
	} else if (checkName("(")) {
		nextToken();
		result = simple_expr();
		matchName(")");
	} else {

		#ifdef DEBUG
		std::cout << "...5" << std::endl;
		#endif

		hasError = true;
	}

	#ifdef DEBUG
	std::cout << "unary result = " << result << std::endl;
	#endif

	return result;
}




void operation(int &result, std::string op, int value) {
	if (op == "+") result += value;
	else if (op == "-") result -= value;
	else if (op == "*") result *= value;
	else if (op == "/") {
		if (value == 0) {
			hasError = true;
		} else {
			result /= value;
		}
	}
}

#undef DEBUG




