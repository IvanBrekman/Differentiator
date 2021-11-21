cr:
	clear
	gcc main.cpp libs/baselib.cpp libs/tree.cpp -lstdc++ -o main.out
	./main.out

c:
	gcc main.cpp libs/baselib.cpp libs/tree.cpp -o main.out

r:
	./main.out
