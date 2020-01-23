a.out : query.o responses.o bus_manager.o main.o
	g++ query.o responses.o bus_manager.o main.o

query.o : query.cpp
	g++ -std=c++14 -c query.cpp

responses.o : responses.cpp
	g++ -std=c++14 -c responses.cpp

bus_manager.o : bus_manager.cpp
	g++ -std=c++14 -c bus_manager.cpp

main.o : main.cpp
	g++ -std=c++14 -c main.cpp

clean:
	rm -rvf *.o
