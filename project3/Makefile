p3: main.o binomialtree.o
	g++ -o p3 main.o binomialtree.o -lQuantLib

main.o: main.cpp
	g++ -o main.o -c -std=c++11 -Wall main.cpp

binomialtree.o: binomialtree.cpp
	g++ -o binomialtree.o -c -std=c++11 -Wall binomialtree.cpp
