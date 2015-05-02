#include "column.h"

Column::Column(std::string name, std::vector<int> element, 
					bool hasDefault, int defaultV) {
	this->name = name;
	this->element = element;
	this->hasDefault = hasDefault;
	this->defaultV = defaultV; 
}

std::string Column::getName() {
	return name; 
}

std::vector<int> Column::getElements() {
    return element;
}

int Column::getValue(int pos) {
    return element[pos];
}

int Column::getPosition(int primary) {
	int pos = 0;
	std::vector<int>::iterator i;
	for(i = element.begin(); i != element.end(); i++, pos++) {
		if (*i == primary)
			break;
	}
	return pos;
}

bool Column::getHasDefault() {
	return hasDefault;
}

int Column::getDefaultValue() {
	return defaultV;
}

/*
 * replace vector with new vector
 */
void Column::setElement(std::vector<int> newElement) {
	this->element = newElement;
}

/*
 * insert element at the back of vector
 */
void Column::insert(int value) {
	element.push_back(value);
}

/*
 * find the position and delete it
 */
void Column::deleteV(int pos) {
	int index = 0;
	std::vector<int>::iterator i;
	for(i = element.begin(); i != element.end(); i++, index++) {
		if (index == pos) {
  			element.erase(i);
  			break;
  		}
	}
}

// delete all elements
void Column::deleteAll() {
    element.clear();
}
