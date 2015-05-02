#include <stdlib.h>
#include "lexer.h"
#include "token.h"

std::string keywords[] = {"create", "table", "int", "default", "primary", 
		"key", "insert", "into", "values", "delete", "from", "where", "select"};

Lexer::Lexer() {}

std::vector<Token> Lexer::getToken(std::ifstream& file) {
	std::vector<Token> tokens;
	std::string line;
	std::string tempStr;
	
	// get every lines of input file
	while (getline(file, line)) {
		// clear the temp string
		tempStr = "";
		while (line.size() > 0) {
			char first = line[0]; 
			// ÅÐ¶ÏÊ××Ö·û£¬Ñ¡ÔñÆ¥ÅäÄ£Ê½
			// space, shift
			if (first == ' ' || first == '\t' || first == '\r' || first == '\n') {
				line = line.erase(0, 1);
			}
			// number
			else if (first >= '0' && first <= '9') {
				int len = getDigits(line, tokens);
				line = line.erase(0, len);
			}
			// id or keywords
			else if (first == '_' || 
					(first >= 'A' && first <= 'Z') ||
					(first >= 'a' && first <= 'z')) {
				int len = getId_Keyword(line, tokens);
				line = line.erase(0, len);	
			}
			// single operator
			else if (first == ',' || first == ';' ||
					 first == '(' || first == ')' ) {
				tempStr += first;
				Token t("operator", tempStr, -1);
				tokens.push_back(t);
				line = line.erase(0, 1);
				
				tempStr = tempStr.erase(0, 1);
			}
			
			// relational operator
			else if (first == '=' || first == '<' || first == '>') {
				int len = getOperator(line, tokens);
				line = line.erase(0, len);
			}
			
			// logical operator
			else if (first == '|' || first == '&' || first == '!') {
			    int len = getLop(line, tokens);
			    line = line.erase(0, len);
			}
			
			// arithmetical operator
			else if (first == '+' || first == '-' || first == '/' || first == '*') {
			    tempStr += first;
				Token t("aop", tempStr, -1);
				tokens.push_back(t);
				line = line.erase(0, 1);
				
				tempStr = tempStr.erase(0, 1);
			}
			
			// error string
			else {
				tempStr += first;
				Token t("error", tempStr, -1);
				tokens.push_back(t);
				line = line.erase(0, 1);
			}
		}
	}
	return tokens;
}

// return the length of token
int getDigits(std::string str, std::vector<Token>& tokens) {
	std::string tempStr = "";
	bool isError = false;
	
	for (int i = 0; i < str.size(); i++) {
		if (str[i] >= '0' && str[i] <= '9') {
			// add it to token
			tempStr += str[i];
		}
		
		// digits must be ended with non-alphabet characters
		else if (!(str[i] >= 'a' && str[i] <= 'z') &&
				 !(str[i] >= 'A' && str[i] <= 'Z')) {
			break;
		}
		// must be error
		else {
			tempStr += str[i];
			isError = true;
		}
	}
	
	if (isError) {
		Token t("error", tempStr, -1);
		tokens.push_back(t);
	} 
	else {
		int number = atoi( tempStr.c_str() );
		Token t("num", "", number);
		tokens.push_back(t);
	}
	
	return tempStr.size();
}

int getId_Keyword(std::string str, std::vector<Token>& tokens) {
	std::string tempStr = "";
	bool isId = false;
	bool isError = false;
	
	for (int i = 0; i < str.size(); i++) {
		// underline or digits
		if (str[i] == '_' || (str[i] >= '0' && str[i] <= '9')) {
			tempStr += str[i];
			isId = true;
		}

		// letters
		else if ((str[i] >= 'A' && str[i] <= 'Z') ||
				 (str[i] >= 'a' && str[i] <= 'z')) {
			tempStr += str[i];	 	
		}
		// end of scanning
		
		else if (!(str[i] >= 'A' && str[i] <= 'Z')
				 && !(str[i] >= 'a' && str[i] <= 'z')
				 && !(str[i] >= '0' && str[i] <= '9')
				 && str[i] != '_') {
			break;
		}
		// error string
		else {
			tempStr += str[i];
			isError = true;
		}	
	}
	if (isError) {
		Token t("error", tempStr, -1);
		tokens.push_back(t);
	} 
	else {
		// check whether it's keyword
		if (!isId) {
			bool isKeyword = false;
			// trans it to lower case
			std::string str = transLow(tempStr);
			
			for (int i = 0; i < 13; i++) {
				if (keywords[i].compare(str) == 0)
					isKeyword = true;
			}
			if (isKeyword) {
				Token t("keyword", tempStr, -1);
				tokens.push_back(t);
			} else {
				Token t("id", tempStr, -1);
				tokens.push_back(t);
			}
		}
		else {
			Token t("id", tempStr, -1);
			tokens.push_back(t);
		}
	}
	
	return tempStr.size();
}

int getOperator(std::string str, std::vector<Token>& tokens) {
	std::string tempStr = "";
	bool isError = false;
	int size = str.size();
	
	for (int i = 0; i < size; i++) {
		// token : "=="
		if (size > 1 && str[i] == '=' && str[i+1] == '=') {
			tempStr = tempStr + str[i] + str[i+1];
		}
		// token : "<>" or "<="
		else if (size > 1 && str[i] == '<' && 
				(str[i+1] == '>' || str[i+1] == '=')) {
			tempStr = tempStr + str[i] + str[i+1];
		}
		// token : ">="
		else if (size > 1 && str[i] == '>' && str[i+1] == '=') {
			tempStr = tempStr + str[i] + str[i+1];
		}
		// token : "="
		else if (str[i] == '=') {
			tempStr += str[i];
			
			Token t("operator", tempStr, -1);
		    tokens.push_back(t);
		    return tempStr.size();
		}
		// single op
		else {
			tempStr += str[i];
		}
		break;
	}
	if (isError) {
		Token t("error", tempStr, -1);
		tokens.push_back(t);
	}
	else {
		Token t("rop", tempStr, -1);
		tokens.push_back(t);
	}
	return tempStr.size();
}

int getLop(std::string str, std::vector<Token>& tokens) {
	std::string tempStr = "";
	bool isError = false;
	int size = str.size();
	
	for (int i = 0; i < size; i++) {
		// token : "||"
		if (size > 1 && str[i] == '|' && str[i+1] == '|') {
			tempStr = tempStr + str[i] + str[i+1];
		}
		// token : "&&"
		else if (size > 1 && str[i] == '&' && str[i+1] == '&') {
			tempStr = tempStr + str[i] + str[i+1];
		}
		// token : "!"
		else if (str[i] == '!') {
			tempStr = tempStr + str[i];
		}
		// token : "&&"
		else {
		    isError = true;
		}
		break;
	}
	if (isError) {
		Token t("error", tempStr, -1);
		tokens.push_back(t);
	}
	else {
		Token t("lop", tempStr, -1);
		tokens.push_back(t);
	}
	return tempStr.size();
}

std::string transLow(std::string str) {

    for (int i = 0; i < str.size(); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] = str[i] + 32;
    }
    return str;
}


