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
	gcc diff/differentiator.cpp simp/simplifier.cpp tex_maker/latex.cpp libs/baselib.cpp libs/tree.cpp -lstdc++ -lm -o diff/differentiator.out
	  ./diff/differentiator.out logs/func.txt

dc:
	gcc diff/differentiator.cpp simp/simplifier.cpp tex_maker/latex.cpp libs/baselib.cpp libs/tree.cpp -lstdc++ -lm -o diff/differentiator.out
