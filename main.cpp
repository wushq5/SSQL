#include <iostream>
#include <fstream>
#include <string>

#include "lexer.h"
#include "insert.h"
#include "delete.h"
#include "create.h"
#include "query.h"

using namespace std;

int main(int argc, char* argv[]) {
	// open file
	ifstream file(argv[1]);
	Lexer lexer;
	
	// create database db called "SSQL"
	// create null vector<Table>
    vector<Table> tableVector;
    Database db("SSQL", tableVector);
	
	// use Lexer to get tokens
	vector<Token> tokens = lexer.getToken(file);
    
    
    // separate every "line"
    vector<Token> tokenLine;
    int commandCount = 0;
    while (!tokens.empty()) {
        bool findDouhao = false;
    
        vector<Token>::iterator iter;
        for (iter = tokens.begin(); iter != tokens.end(); iter++) {
            
            // indicate the end of line, cut it
            if ((*iter).getName().compare(";") == 0) {
                // copy to new vector
                tokenLine.assign(tokens.begin(), ++iter);
                // delete the line
                tokens.erase(tokens.begin(), iter);
                findDouhao = true;
                break;
            }
        }
        
        if (!findDouhao) {
            cout << "Lack of ';'." << endl;
            break;   
        }
        
        commandCount++;
        for (iter = tokenLine.begin(); iter != tokenLine.end(); iter++) {
            if ((*iter).getType() == "error") {
                cout << "In NO." << commandCount << " command contains error token: [" 
                     <<  (*iter).getName() << "]." << endl;
            }
        }
        
        string first = tokenLine[0].getName();
        first = TransLow(first);
        
        if (first == "create") {
                Create creator(tokenLine);
                Table t = creator.create();
                if (db.isTableExist(t.getName())) {
                    creator.printTest(true);
                } else {
                    creator.printTest(false);
                    db.addTable(t);
            }
            
        }
        else if (first == "insert") {
            // fill in insert code
            insert(tokenLine, db);
        
        }
        else if (first == "delete") {
            deleteParse(db, tokenLine);
        }
        else if (first == "select") {
            // fill in query code
             queryParse(db, tokenLine);
        }
        else {
            cout << "Syntax Error.\n";        
            
        }
        
        
        // clear content in vector
        tokenLine.clear();
    }
	
	return 0;
}

