main: main.o translator.o
	g++ main.o translator.o -o translator.exe
main.o: main.cpp
	g++ -std=c++0x main.cpp -c -o main.o
translator.o: translator.cpp
	g++ -std=c++0x translator.cpp -c -o translator.o
clean:
	rm -rf main.o
	rm -rf translator.o
	rm -rf translator.exe