main: main.o vmashine.o
	g++ main.o vmashine.o -o interpretator.exe
main.o: main.cpp
	g++ -std=c++0x main.cpp -c -o main.o
vmashine.o: vmashine.cpp
	g++ -std=c++0x vmashine.cpp -c -o vmashine.o
clear:
	rm -rf main.o
	rm -rf vmashine.o
	rm -rf interpretator.exe