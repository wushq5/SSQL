#include <iostream>
#include <math.h>	// isinf(), isnan()

#include "delete.h"

using namespace std;

//#define DEBUG

Token lookahead("", "", 0);
vector<Token> tokens;
int tokenIndex;
bool hasError;

Table *currentTable = NULL;	// operate table


void deleteParse(Database& db, vector<Token> _tokens) {
    
    tokenIndex = 0; 
	hasError = false;
	currentTable = NULL;

	lookahead = _tokens.front();
	tokens = _tokens;
	
	std::cout << std::boolalpha;

	// predictive
	matchName("delete");
	matchName("from");

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

	std::vector<int> rowIndex = where_clause();
	if (hasError) {
		std::cerr << "Expression error." << std::endl;
		return;
	}

	matchName(";");
	
	if (!hasError) {
	    //deleteOperation(db, id, number, rop);
	    deleteOperation(db, rowIndex);
	    cout << "Delete operation success!" << endl;

	} else {
	    cout << "Syntax error!" << endl;
	}
	
}


std::vector<int> where_clause() {

	#ifdef DEBUG
	/**/std::cout << "where_clause()" << std::endl;
	#endif

	std::vector<bool> mask;
	if (checkName("where")) {
		// if there's a "where" token
		nextToken();		// consume this token
		mask = disjunct();	// compute disjunct and return
	} else {
		// else, consume no token and returns all true
		mask = genVector(true);
	}

	std::vector<int> choose;
	for (int i = mask.size() - 1; i >= 0; i--) {
		if (mask[i] == true) choose.push_back(i);
	}

	return choose;
}

std::vector<bool> disjunct() {
	std::vector<bool> result = genVector(false);
	std::string op = "||";
	bool stopFlag = false;

	#ifdef DEBUG
	/**/std::cout << "disjunct()" << std::endl;
	#endif

	for (; !stopFlag && !hasError;) {
		// calculate the result
		if (checkType("id") || checkType("num") || checkName("(")
			|| checkName("!") || checkName("+") || checkName("-")) {
			compare(result, op, conjunct());
		} else {
			hasError = true;
		}

		// if there's any "||" token, continue the calculation
		if (checkName("||")) {
			op = lookahead.getName();
			nextToken();
		} else if (checkName(")") || checkName(";")) {
			stopFlag = true;
		} else {
			hasError = true;
		}
	}

	#ifdef DEBUG
	/**/std::cout << "~disjunct() " << hasError << std::endl;
	#endif
	return result;
}

std::vector<bool> conjunct() {
	std::vector<bool> result = genVector(true);
	std::string op = "&&";
	bool stopFlag = false;

	#ifdef DEBUG
	/**/std::cout << "conjunct()" << std::endl;
	#endif

	for (; !stopFlag && !hasError;) {
		// calculate the result
		if (checkType("id") || checkType("num") || checkName("(")
			|| checkName("!") || checkName("+") || checkName("-")) {
			compare(result, op, _bool());
		} else {
			hasError = true;
		}

		// if there's any "&&" token, continue the calculation
		if (checkName("&&")) {
			op = lookahead.getName();
			nextToken();
		} else if (checkName("||") || checkName(")") || checkName(";")) {
			stopFlag = true;
		} else {
			hasError = true;
		}
	}

	#ifdef DEBUG
	/**/std::cout << "~conjunct() " << hasError << std::endl;
	#endif
	return result;
}

std::vector<bool> _bool() {
	std::vector<bool> result;

	#ifdef DEBUG
	/**/std::cout << "_bool()" << std::endl;
	#endif

	if (checkType("id") || checkType("num") || checkName("+")
		|| checkName("-")) {
		// rule: _bool -> comp
		result = comp();
	} else if (checkName("(")) {
		// rule: _bool -> ( disjunct )
		nextToken();
		result = disjunct();
		matchName(")");
	} else if (checkName("!")) {
		// rule: _bool -> ! bool
		nextToken();
		compare(result, "!", _bool());
	} else {
		hasError = true;
	}

	#ifdef DEBUG
	/**/std::cout << "~_bool() " << hasError << std::endl;
	#endif
	return result;
}

std::vector<bool> comp() {
	#ifdef DEBUG
	/**/std::cout << "comp()" << std::endl;
	#endif
	std::vector<double> lhs = expr();
	std::string op;
	if (checkType("rop")) {
		op = lookahead.getName();
		nextToken();
		#ifdef DEBUG
		/**/std::cout << "rop()" << std::endl;
		#endif
	} else {
		hasError = true;
	}
	std::vector<double> rhs = expr();
	#ifdef DEBUG
	/**/std::cout << "~comp() " << hasError << std::endl;
	#endif
	return compare(lhs, op, rhs);
}

std::vector<double> expr() {
	std::vector<double> result = genVector(0.0);
	std::string op = "+";
	bool stopFlag = false;

	#ifdef DEBUG
	/**/std::cout << "expr()" << std::endl;
	#endif

	for (; !stopFlag && !hasError;) {
		// calculate the result
		if (checkType("id") || checkType("num") || checkName("+")
			|| checkName("-")) {
			operation(result, op, term());
		} else {
			hasError = true;
		}

		// if there's any "+" or "-" token, continue the calculation
		if (checkName("+") || checkName("-")) {
			op = lookahead.getName();
			nextToken();
		} else if (checkType("rop") || checkName("&&") || checkName("||")
					|| checkName(")") || checkName(";")) {
			stopFlag = true;
		} else {
			hasError = true;
		}
	}

	#ifdef DEBUG
	/**/std::cout << "~expr() " << hasError << std::endl;
	#endif

	return result;
}

std::vector<double> term() {
	std::vector<double> result = genVector(1.0);
	std::string op = "*";
	bool stopFlag = false;

	#ifdef DEBUG
	/**/std::cout << "term()" << std::endl;
	#endif

	for (; !stopFlag && !hasError;) {
		// calculate the result
		if (checkType("id") || checkType("num") || checkName("+")
			|| checkName("-")) {
			operation(result, op, unary());
		} else {
			hasError = true;
		}

		// if there's any "*" or "/" token, continue the calculation
		if (checkName("*") || checkName("/")) {
			op = lookahead.getName();
			nextToken();
		} else if (checkType("rop") || checkName("&&") || checkName("||")
					|| checkName("+") || checkName("-") || checkName(")")
					|| checkName(";")) {
			stopFlag = true;
		} else {
			hasError = true;
		}
	}

	#ifdef DEBUG
	/**/std::cout << "~term() " << hasError << std::endl;
	#endif

	return result;
}

std::vector<double> unary() {
	std::vector<double> result;

	#ifdef DEBUG
	/**/std::cout << "unary()" << std::endl;
	#endif

	if (checkName("+") || checkName("-")) {
		result = genVector(0.0);
		std::string op = lookahead.getName();
		nextToken();
		operation(result, op, unary());
	} else if (checkType("id")) {
		result = genVector(lookahead.getName());
		nextToken();
	} else if (checkType("num")) {
		result = genVector((double)lookahead.getNum());
		nextToken();
	} else {
		hasError = true;
	}

	#ifdef DEBUG
	/**/std::cout << "~unary() " << hasError << std::endl;
	#endif

	return result;
}


// generate a vector with same values
std::vector<bool> genVector(bool value) {
	std::vector<bool> vect;
	if (currentTable != NULL) {
		for (int i = 0; i < currentTable->getNumberOfRows(); i++) {
			vect.push_back(value);
		}
	}
	return vect;
}

std::vector<double> genVector(double value) {
	std::vector<double> vect;
	if (currentTable != NULL) {
		for (int i = 0; i < currentTable->getNumberOfRows(); i++) {
			vect.push_back(value);
		}
	}
	return vect;
}

// get column id and return a vector
std::vector<double> genVector(std::string columnName) {
	std::vector<double> vect;
	if (currentTable != NULL) {
		if (currentTable->isColumnExist(columnName)) {
			Column &column = currentTable->getColumnReference(columnName);
			for (int i = 0; i < currentTable->getNumberOfRows(); i++) {
				vect.push_back(column.getValue(i));
			}
		} else {
			hasError = true;
			std::cerr << "Column ["  << columnName;
			std::cerr << "] doesn't exist." << std::endl;
			return vect;
		}
	}
	return vect;
}

// accumulative logical operation
void compare(std::vector<bool> &result, std::string op,
			 std::vector<bool> value) {
	#ifdef DEBUG
	/**/std::cout << "compare() op: " << op << std::endl;
	/**/std::cout << "          result: " << result.size() << std::endl;
	/**/std::cout << "          value: " << value.size() <<std::endl;
	#endif
	if (op == "!") {
		result.clear();
		for (int i = 0; i < value.size(); i++) {
			result.push_back(!value[i]);
		}
	} else if (result.size() != value.size()) {
		hasError = true;
	} else {
		for (int i = 0; i < result.size(); i++) {
			if (op == "&&") {
				result[i] = result[i] && value[i];
			} else if (op == "||") {
				result[i] = result[i] || value[i];
			} else {
				#ifdef DEBUG
				/**/std::cout << "          hasError!" <<std::endl;
				#endif
				hasError = true;
				break;
			}
		}
	}
}

// relation operation
std::vector<bool> compare(std::vector<double> lhs,
			std::string op, std::vector<double> rhs) {
	std::vector<bool> result;
	#ifdef DEBUG
	/**/std::cout << "compare() op: " << op << std::endl;
	/**/std::cout << "          lhs: " << lhs.size() << std::endl;
	/**/std::cout << "          rhs: " << rhs.size() <<std::endl;
	#endif
	if (lhs.size() != rhs.size()) {
		hasError = true;
	} else {
		for (int i = 0; i < lhs.size(); i++) {
			if (op == "<") {
				result.push_back(lhs[i] < rhs[i]);
			} else if (op == ">") {
				result.push_back(lhs[i] > rhs[i]);
			} else if (op == "==") {
				result.push_back(lhs[i] == rhs[i]);
			} else if (op == "<>") {
				result.push_back(lhs[i] != rhs[i]);
			} else if (op == ">=") {
				result.push_back(lhs[i] >= rhs[i]);
			} else if (op == "<=") {
				result.push_back(lhs[i] <= rhs[i]);
			} else {
				#ifdef DEBUG
				/**/std::cout << "          hasError!" <<std::endl;
				#endif
				hasError = true;
				break;
			}
		}
	}
	return result;
}

// accumulative arithmetic operation
void operation(std::vector<double> &result, std::string op,
				std::vector<double> value) {
	if (result.size() != value.size()) {
		hasError = true;
	} else {
		// here we provide an int-like result
		// such as 8/5 returns 1 but not 1.6
		// but we reserved some good characteristic of double
		// that dividedByZero error won't crash our program
		for (int i = 0; i < result.size(); i++) {
			if (op == "+") {
				result[i] += value[i];
			} else if (op == "-") {
				result[i] -= value[i];
			} else if (op == "*") {
				result[i] *= value[i];
			} else if (op == "/") {
				result[i] /= value[i];
				// need for an int-cast result
				if (!isinf(result[i] && !isnan(result[i]))) {
					result[i] = (int)result[i];
				} 
			} else {
				hasError = true;
				break;
			}
		}
	}
}


// just check the type of the current token
bool checkType(std::string type) {
	return lookahead.getType() == type;
}

// just check the name of the current token
bool checkName(std::string name) {
	std::string lookaheadName = lookahead.getName();
	if (checkType("keyword"))
		lookaheadName = TransLow(lookaheadName);
	return lookaheadName == name;
}

// move to next token
void nextToken() {
	lookahead = tokens[tokenIndex + 1];
	tokenIndex++;
	#ifdef DEBUG
	/**/std::cout << "   lookahead: ";
	/**/std::cout << lookahead.getType() << " ";
	/**/std::cout << lookahead.getName() << " ";
	/**/std::cout << lookahead.getNum() << std::endl;
	#endif
}

// match a token by type and move to the next token
void matchType(string target) {

	string str = lookahead.getType();
	if (str.compare(target) == 0) {
		if (tokenIndex + 1 < tokens.size()) {
			nextToken();	

		}

	} else
		hasError = true;	
}

// match a token by name and move to the next token
void matchName(string target) {

	string str = lookahead.getName();
	// keyword id case-insensitive
	if (lookahead.getType() == "keyword")
		str = TransLow(str);
		
	if (str.compare(target) == 0) {
		if (tokenIndex + 1 < tokens.size()) {
			nextToken();
		}
		
	} else
		hasError = true;
		
}
 
// change higher case to lower case
std::string TransLow(std::string str) {

    for (int i = 0; i < str.size(); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] = str[i] + 32;
    }
    return str;
}


void deleteOperation(Database &db, std::vector<int> rowIndex) {
	currentTable->deleteRows(rowIndex);
}

#undef DEBUG


