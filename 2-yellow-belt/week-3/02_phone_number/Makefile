a.out : main.o phone_number.o
	g++ main.o phone_number.o

main.o : main.cpp
	g++ -std=c++14 -c main.cpp

phone_number.o : phone_number.cpp
	g++ -std=c++14 -c phone_number.cpp

clean:
	rm -rfv *.o
