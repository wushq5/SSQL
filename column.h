#ifndef COLUMN_H_
#define COLUMN_H_

#include <iostream>
#include <vector>


class Column {
	public:
		Column(std::string name, std::vector<int> element, bool hasDefault, int defaultV);
		std::string getName();
		std::vector<int> getElements();
		int getPosition(int primaryValue);
		int getValue(int pos);
		bool getHasDefault();
		int getDefaultValue();
		void setElement(std::vector<int> newColumn);
		void insert(int value);
		void deleteV(int pos);
		void deleteAll();       //delete all elements
		
	private:
		// column name
		std::string name;
		// store column value
		std::vector<int> element;
		// indicate whether it has default value
		bool hasDefault;
		// default value
		int defaultV;
};

#endif 
