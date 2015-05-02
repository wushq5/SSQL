#ifndef TOKEN_H_
#define TOKEN_H_

#include <iostream>
#include <string>


class Token {
	public:
		Token(std::string type = "", std::string name = "", int num = 0);
		std::string getType();
		std::string getName();
		int getNum();
		void setType(std::string type);
		void setName(std::string name);
		void setNum(int num);
		
		
	protected:
		std::string type;
		std::string name;
		int num;
};

#endif
