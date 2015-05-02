
#include "create.h"
using namespace std;

extern Token lookahead; //the ahead token

extern std::vector<Token> tokens;//all tokens of each sentence
int tcount;	//current index of tokens

string tableName; //name of a table
string tempColName; //name of a column
string primaryKey;	// one of the primary keys

vector<string> pKeys; //all primary keys in a table
bool hasPrimaryKey;	//judge if the table has any primary key

bool hasDefault; // if a column has default:true, else false
int defaultV; // default value, if a column has no default, defaultV = 0

vector<Column> cols; // all columns in a table
vector<string> expr;// expression in default value in order

bool errorDeteced;	//judge if there is an error
bool isNegative; //judge if the num is a negative

Create::Create(vector<Token> token){
	tokens = token;
	tcount = 0;
	hasDefault = false;
	defaultV = 0;
	hasPrimaryKey = false;
	errorDeteced = false;
	isNegative = false;

	tokens.clear();
	pKeys.clear();
	cols.clear();
	expr.clear();

	lookahead = getNextToken();
}

Table Create::create() {
	if (create_stmt()) {
		Table table(tableName, cols, pKeys);
		if (detectError(table)) {//error exists
			Table t("", cols, pKeys);
			return t;
		} else {
			return table;
		}
	}
	Table t("", cols, pKeys);
	return t;
}
bool detectError(Table table) {//true indicates error found
	if (cols.size() > 100) {
		errorDeteced = true;
		cout << "error: number of columns > 100" << endl;
		return true;
	}
	if (pKeys.size() > 100) {
		errorDeteced = true;
		cout << "error: number of primary key declarations > 100" << endl;
		return true;
	}

	vector<Column> tmp1(cols);
	for (int i = 0; i < cols.size(); i++) {
		//judge if there are duplicate column names.
		for (int j = 0; j < cols.size(); j++) {
			if (j != i) {
				if (tmp1[i].getName() == tmp1[j].getName()) {
					errorDeteced = true;
					cout << "error: duplicate column name: \"" << tmp1[i].getName() << "\"" << endl;
					return true;
				}
			}
		} 
	}

	vector<string> tmp2(pKeys);
	sort(tmp2.begin(), tmp2.end());
	//judge if primary key id is in the columns.
	for (int i = 0; i < pKeys.size(); i++) {
		if (!table.isColumnExist(pKeys[i])) {
			errorDeteced = true;
			cout << "error: primary key not found: \"" << pKeys[i] << "\"" << endl;
			return true;
		}
		//judge if there are duplicate primary keys.
		if (i == pKeys.size() - 1) { break;}
		if (tmp2[i] == tmp2[i + 1]) {
			errorDeteced = true;
			cout << "error: primary keys duplicate: \"" << tmp2[i] << "\"" << endl;
			return true;
		} 
	}

	return false; //indicate no error found
}
//print result
void Create::printTest(bool exist) {
	if (errorDeteced) {
		cout << "failed to create table \"" << tableName << "\"" << endl << endl;
		return;
	}
	if (exist) {
		cout << "failed to create table \"" << tableName << "\"" << endl;
        cout << "error: table \"" << tableName << "\" already exist" << endl;
        return;
	}
	if (!errorDeteced) {
		cout << "SUCCESS: create table \"" << tableName << "\"" << endl;
		
		//test example for create statment    
	    cout << "table: " << tableName << endl;
	    cout << "primary keys: ";
	    for (int k = 0; k < pKeys.size(); k++) {
	        cout << pKeys[k];
	        if (pKeys.size() > 1 && k != pKeys.size() - 1) {
	            cout << " | ";
	        }
	    }
	    cout << endl;
	    cout << "|";
	    for (int i = 0; i < cols.size(); i++) {
	    	cout << "------------------|";
	    }
	    cout << endl;
	    for (int j = 0; j < cols.size(); j++) {
	    	int length = cols[j].getName().size() + (cols[j].getHasDefault() ? (5 + intToStr(cols[j].getDefaultValue()).size()) : 0);

	        cout << "|";
	        for (int m = 0; m < (19 -  length) / 2; m++) {
	        	cout << " ";
	        }
	        cout << cols[j].getName();
	        if (cols[j].getHasDefault()) {
	            cout << " def=" << cols[j].getDefaultValue();
	        }
	        for (int m = 0; m < (19 - (length + 1)) / 2; m++) {
	        	cout << " ";
	        }
	    }
	    cout << "|" << endl;
	    cout << "|";
	    for (int i = 0; i < cols.size(); i++) {
	    	cout << "------------------|";
	    }
	 	cout << endl << endl;
	} else {
		cout << "failed to create table \"" << tableName << "\"" << endl << endl;
	}
}

bool create_stmt() {
	if (transLow(lookahead.getName()) == "create") {
		match("keyword", "create"); 
		match("keyword", "table"); 
		tableName = lookahead.getName(); match("id", ""); 
		match("operator", "(");
		decl_list();
		match("operator", ")"); 
		match("operator", ";");
		if (errorDeteced) return false;
		else return true;
	} else {
		cout << "syntax error: can't find keyword [create]" << endl;
		errorDeteced = true;
		return false;
	}
}

void decl_list() {
	if (errorDeteced) return;
	decl();
	decl_list2();
}

void decl_list2() {
	if (errorDeteced) return;
	if (lookahead.getName() == ",") {
		match("operator", ",");
		decl();
		decl_list2();
	}
}

void decl() {
	if (errorDeteced) return;
	if (lookahead.getType() == "id") {
		tempColName = lookahead.getName();
		match("id", "");
		match("keyword", "int");
		default_spec();
	} else if (transLow(lookahead.getName()) == "primary") {
		match("keyword", "primary");
		match("keyword", "key");
		if (hasPrimaryKey) {
			cout << "error: duplicate primary key declarations" << endl;
			errorDeteced = true;
			return;
		}
		hasPrimaryKey = true;
		match("operator", "(");
		column_list();
		match("operator", ")");
	} else {
		cout << "syntax error: can't find type [id] or keyword [primary] in columns." << endl;
		errorDeteced = true;
	}
}

void default_spec() {
	if (errorDeteced) return;
	if (transLow(lookahead.getName()) == "default") {
		hasDefault = true;
		match("keyword", "default");
		match("operator", "=");
		Csimple_expr();
		//set default value
		defaultV = calculateRPN(RPN(expr));
		expr.clear();
	} else {
		defaultV = 0;
		hasDefault = false;
	}
	
	vector<int> value;
	//create a column
	Column column(tempColName, value, hasDefault, defaultV);
	cols.push_back(column);
}

void Csimple_expr() {
	if (errorDeteced) return;
	Csimple_term();
	Csimple_expr2();
}

void Csimple_expr2() {
	if (errorDeteced) return;
	if (lookahead.getName() == "+") {
		expr.push_back("+");
		match("aop", "+");
		Csimple_term();
		Csimple_expr2();
	} else if (lookahead.getName() == "-") {
		expr.push_back("-");
		match("aop", "-");
		Csimple_term();
		Csimple_expr2();
	}
	
}

void Csimple_term() {
	if (errorDeteced) return;
	Csimple_unary();
	Csimple_term2();
}

void Csimple_term2() {
	if (errorDeteced) return;
	if (lookahead.getName() == "*") {
		expr.push_back("*");
		match("aop", "*");
		Csimple_unary();
		Csimple_term2();
	} else if (lookahead.getName() == "/") {
		expr.push_back("/");
		match("aop", "/");
		Csimple_unary();
		Csimple_term2();
	}
}

void Csimple_unary() {
	if (errorDeteced) return;
	if (lookahead.getName() == "(") {
		expr.push_back("(");
		match("operator", "(");
		Csimple_expr();
		expr.push_back(")");
		match("operator", ")");
	} else if (lookahead.getName() == "-") {
		isNegative = true;
		match("aop", "-");
		Csimple_unary();
	} else if (lookahead.getName() == "+") {
		isNegative = false;
		match("aop", "+");
		Csimple_unary();
	} else if (lookahead.getType() == "num") {
		defaultV = lookahead.getNum() * (isNegative ? -1 : 1);
		expr.push_back(intToStr(defaultV));
		match("num", "");
		isNegative = false;
	} else {
		errorDeteced = true;
		cout << "syntax error: expression error" << endl;
	}
}


void column_list() {
	if (errorDeteced) return;
	if (lookahead.getType() == "id") {
		primaryKey = lookahead.getName();
		match("id", "");
		column_list2();
	} else {
		cout << "syntax error: can't find type [id] in primary keys" << endl;
		errorDeteced = true;
	}
}

void column_list2() {
	if (errorDeteced) return;
	if (lookahead.getName() == ",") {
		match("operator", ",");
		pKeys.push_back(lookahead.getName());
		match("id", "");
		column_list2();
	} else {
		pKeys.push_back(primaryKey);
	}

}
//match the predicted token and lookahead
void match(string type, string name) {
	if (errorDeteced) return;
	if (type == "id") {
		if (lookahead.getType() == "id") lookahead = getNextToken();
		else {
			cout << "syntax error: \"" << lookahead.getName() << "\" can't match type [id]" << endl;
			errorDeteced = true;
		}
	} else if (type == "num") {
		if (lookahead.getType() == "num") lookahead = getNextToken();
		else {
			cout << "syntax error: \"" << lookahead.getNum() << "\" can't match type [num]" << endl;
			errorDeteced = true;
		}
	} else if (type == "keyword") {
		if (lookahead.getType() == "keyword") {
			if (transLow(lookahead.getName()) == name) {
				lookahead = getNextToken();
			} else {
				cout << "syntax error: \"" << lookahead.getName() << "\" can't match keyword: \"" << name << "\"" << endl;
				errorDeteced = true;
			}
		} else {
			cout << "syntax error: \"" << lookahead.getType() << "\" can't match type [keyword]" << endl;
			errorDeteced = true;
		}
	} else if (type == "operator") {
		if (lookahead.getType() == "operator") {
			if (lookahead.getName() == name) {
				if (name != ";") {//indicate the last token of each line
					lookahead = getNextToken();
				}
			} else {
				cout << "syntax error: \"" << lookahead.getName() << "\" can't match operator: \"" << name << "\"" << endl;
				errorDeteced = true;
			}
		} else {
			cout << "syntax error: \"" << lookahead.getType() << "\" can't match type [operator]" << endl;
			errorDeteced = true;
		}
	} else if (type == "aop") {
		if (lookahead.getType() == "aop") {
			if (lookahead.getName() == name) {
				lookahead = getNextToken();
			} else {
				errorDeteced = true;
				cout << "syntax error: \"" << lookahead.getName() << "\" can't match operator: \"" << name << "\"" << endl;
			}
		} else {
			cout << "syntax error: \"" << lookahead.getType() << "\" can't match type [aop]" << endl;
			errorDeteced = true;
		}
	} else if (type == "error") {
		cout << "syntax error: " << "type error" << endl;
		errorDeteced = true;
	} else {
		cout << "unfound syntax error" << endl;
		errorDeteced = true;
	}
}

Token getNextToken() {
	return tokens[tcount++];
}
//convert infix expression to suffix expression(Reverse Polish Notation)
stack<string> RPN(vector<string> exp) {
	stack<string> temp;
	temp.push("#");
	stack<string> rpn;//stack restoring the RPN
	//scan the input expression
	for (int i = 0; i < exp.size(); i++) {
		if (exp[i] == "+" || exp[i] == "-") {
			if (temp.top() == "(" || temp.top() == "#") {
				temp.push(exp[i]);
			} else {
				string op = temp.top();
				temp.pop();
				rpn.push(op);
				temp.push(exp[i]);
			}
		} else if (exp[i] == "*" || exp[i] == "/") {
			if (temp.top() == "(" || temp.top() == "#" 
				|| temp.top() == "+" || temp.top() == "-") {
				temp.push(exp[i]);
			} else {
				string op = temp.top();
				temp.pop();
				rpn.push(op);
				temp.push(exp[i]);
			}
		} else if (exp[i] == "(") {
			temp.push(exp[i]);
		} else if (exp[i] == ")") {
			while(temp.top() != "(") {
				string op = temp.top();
				temp.pop();
				rpn.push(op);
			}
			temp.pop();//pop out "("
		} else {//num
			rpn.push(exp[i]);
		}
	}
	while(temp.top() != "#") {
		string op = temp.top();
		temp.pop();
		rpn.push(op);
	}
	return rpn;
}
//calculate rpn to get the result
int calculateRPN(stack<string> rpn) {
	stack<string> rrpn;
	while (!rpn.empty()) {
		string op = rpn.top();
		rrpn.push(op);
		rpn.pop();
	}

	stack<string> temp;
	while (!rrpn.empty()) {
		if (rrpn.top() == "+" || rrpn.top() == "-" || rrpn.top() == "*" || rrpn.top() == "/") {//operator
			int num1 = atoi(temp.top().c_str());
			temp.pop();
			int num2 = atoi(temp.top().c_str());
			temp.pop();
			int result = 0;
			if (rrpn.top() == "+") {
				result = num2 + num1;
			} else if (rrpn.top() == "-") {
				result = num2 - num1;
			} else if (rrpn.top() == "*") {
				result = num2 * num1;
			} else if (rrpn.top() == "/") {
				if (num1 == 0) {
					cout << "error: division by 0" << endl;
					errorDeteced = true;
					return 0;
				}
				result = num2 / num1;
			}
			temp.push(intToStr(result));
		} else {//number
			temp.push(rrpn.top());
		}
		rrpn.pop();
	}
	int res = atoi(temp.top().c_str());
	return res;
}
//convert type int to type string
string intToStr(int num) {
	stringstream ss;
	string str;
	ss << num;
	ss >> str;
	return str;
}

