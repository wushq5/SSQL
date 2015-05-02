#include "token.h"

Token::Token(std::string type, std::string name, int num) {
	this->type = type;
	this->name = name;
	this->num = num;
}


std::string Token::getType() {
	return type;
}

std::string Token::getName() {
	return name;
}

int Token::getNum() {
	return num;
}

void Token::setType(std::string tpye) {
	this->type = type;
}

void Token::setName(std::string name) {
	this->name = name;
}

void Token::setNum(int num) {
	this->num = num;
}



