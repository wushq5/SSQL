#ifndef LEXER_H_
#define LEXER_H_

#include <iostream>
#include <fstream>
#include <vector>

#include "token.h"

class Lexer {
	public:
		Lexer();
		std::vector<Token> getToken(std::ifstream& file);
		
	private:
		
};

int getDigits(std::string str, std::vector<Token>& tokens);
int getId_Keyword(std::string str, std::vector<Token>& tokens);
int getOperator(std::string str, std::vector<Token>& tokens);
int getLop(std::string str, std::vector<Token>& tokens);

std::string transLow(std::string str);

#endif
