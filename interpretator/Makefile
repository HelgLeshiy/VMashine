main: main.o
	g++ main.o -o interpretator.exe
main.o: main.cpp
	g++ -std=c++0x main.cpp -c -o main.o
clear:
	rm -rf main.o
	rm -rf interpretator.exe