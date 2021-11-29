cr:
	clear
	gcc main.cpp libs/baselib.cpp libs/tree.cpp -lstdc++ -o main.out
	./main.out

c:
	gcc main.cpp libs/baselib.cpp libs/tree.cpp -lstdc++ -o main.out

r:
	./main.out


dcr:
	clear
	gcc differentiator/differentiator.cpp libs/baselib.cpp libs/tree.cpp -lstdc++ -lm -o differentiator/differentiator.out
	  ./differentiator/differentiator.out function.txt
