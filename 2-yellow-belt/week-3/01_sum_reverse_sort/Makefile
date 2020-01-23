a.out : main.o sum_reverse_sort.o
	g++ main.o sum_reverse_sort.o

main.o : main.cpp
	g++ -std=c++14 -c main.cpp

sum_reverse_sort.o : sum_reverse_sort.cpp
	g++ -std=c++14 -c sum_reverse_sort.cpp

clean:
	rm -f *.o
