SRC=src
UNICODE=-D_UNICODE

all:	mexpr clean.o


mexpr:	main.o CStringOp.o CMyParser.o CExprParser.o
	g++ main.o CStringOp.o CMyParser.o CExprParser.o -o mexpr

main.o:
	g++ $(UNICODE) -c $(SRC)/main.cpp

CStringOp.o:
	g++ $(UNICODE) -c $(SRC)/CStringOp.cpp

CMyParser.o:
	g++ $(UNICODE) -c $(SRC)/CMyParser.cpp

CExprParser.o:
	g++ $(UNICODE) -c $(SRC)/CExprParser.cpp

clean:
	rm -rf *.o mexpr

clean.o:
	rm -rf *.o
