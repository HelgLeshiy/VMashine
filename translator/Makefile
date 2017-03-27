main: main.o translator.o misc.o
	g++ main.o translator.o misc.o -o translator.exe
main.o: main.cpp
	g++ -std=c++0x main.cpp -c -o main.o
translator.o: translator.cpp
	g++ -std=c++0x translator.cpp -c -o translator.o
misc.o: misc.cpp
	g++ -std=c++0x misc.cpp -c -o misc.o
clean:
	rm -rf main.o
	rm -rf translator.o
	rm -rf misc.o
	rm -rf translator.exe