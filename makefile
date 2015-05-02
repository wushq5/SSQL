ss : main.o lexer.o token.o database.o table.o column.o delete.o insert.o create.o query.o
	g++ -o ss main.o lexer.o token.o database.o table.o column.o delete.o insert.o create.o query.o
	
main.o :  main.cpp lexer.h delete.h insert.h create.h query.h
	g++ -c  main.cpp -o main.o
	
lexer.o : lexer.cpp lexer.h token.h
	g++ -c lexer.cpp -o lexer.o
	
token.o : token.cpp token.h
	g++ -c token.cpp -o token.o
	
database.o : database.cpp database.h table.h
	g++ -c database.cpp -o database.o
	
table.o : table.cpp table.h column.h
	g++ -c table.cpp -o table.o
	
column.o : column.cpp column.h
	g++ -c column.cpp -o column.o
	
delete.o : delete.cpp delete.h token.h database.h
	g++ -c delete.cpp -o delete.o
	
insert.o : insert.cpp insert.h token.h database.h lexer.h
	g++ -c insert.cpp -o insert.o
	
create.o : create.cpp create.h token.h database.h
	g++ -c create.cpp -o create.o
	
query.o : query.cpp query.h token.h database.h
	g++ -c query.cpp -o query.o
	
clean:
	rm ss main.o lexer.o token.o  database.o table.o column.o delete.o insert.o create.o query.o
